import argparse
import os.path
from collections import Counter, defaultdict, namedtuple
import pickle
from typing import Dict, List, Literal, Tuple


Target = namedtuple("Target", ["target_id", "node_id", "disk_index"])
Chain = namedtuple("Chain", ["chain_id", "target_list"])


def calc_target_id(target_id_prefix: int, node_id: int, disk_index: int, target_index: int):
  return ((target_id_prefix * 1_000_000 + node_id) * 1_000 + (disk_index+1)) * 100 + (target_index+1)


def generate_chains(
    chain_table_type: Literal["EC", "CR"],
    node_id_begin: int,
    node_id_end: int,
    num_disks_per_node: int,
    num_targets_per_disk: int,
    target_id_prefix: int,
    chain_id_prefix: int,
    incidence_matrix: Dict[Tuple[int, int], bool],
    **kwargs):
  num_nodes = node_id_end - node_id_begin + 1
  nodes, groups = zip(*sorted(incidence_matrix.keys()))
  group_sizes = list(Counter(groups).values())
  assert max(nodes) == num_nodes, f"{max(nodes)=} != {num_nodes=}"
  assert all(s == group_sizes[0] for s in group_sizes[1:]), f"not all group sizes the same: {group_sizes}"
  assert len(incidence_matrix) % group_sizes[0] == 0, f"{len(incidence_matrix)=} % {group_sizes[0]=} != 0"
  assert len(incidence_matrix) == num_nodes * num_targets_per_disk, f"{len(incidence_matrix)=} != {num_nodes=} * {num_targets_per_disk=}"

  global_target_list = []
  chain_target_list = defaultdict(list)

  for disk_index in range(num_disks_per_node):
    group_slot_idx = defaultdict(int)
    for node_id in range(node_id_begin, node_id_end+1):
      for target_index in range(num_targets_per_disk):
        target_id = calc_target_id(target_id_prefix, node_id, disk_index, target_index)
        target_pos = (node_id - node_id_begin) * num_targets_per_disk + target_index

        if chain_table_type == "EC":
          group_slot_idx[groups[target_pos]] += 1
          chain_index = (groups[target_pos]-1) * group_sizes[0] + group_slot_idx[groups[target_pos]]
        else:
          chain_index = groups[target_pos]

        assert chain_index < 1_00_000, f"{chain_index} >= {1_00_000}"
        chain_id = (chain_id_prefix * 1_000 + (disk_index+1)) * 1_00_000 + chain_index
        target = Target(target_id, node_id, disk_index)
        global_target_list.append(target)
        chain_target_list[chain_id].append(target)

  num_targets_on_node = list(Counter(target.node_id for target in global_target_list).values())
  num_targets_on_disk = list(Counter((target.node_id, target.disk_index) for target in global_target_list).values())
  assert len(global_target_list) == len(set(global_target_list)) == num_nodes * num_disks_per_node * num_targets_per_disk
  assert all(x == num_targets_on_node[0] for x in num_targets_on_node[1:])
  assert all(x == num_targets_on_disk[0] for x in num_targets_on_disk[1:])

  if chain_table_type == "EC":
    assert all(len(target_ids) == 1 for target_ids in chain_target_list.values())
    assert len(chain_target_list) == num_nodes * num_disks_per_node * num_targets_per_disk
  else:
    assert all(len(target_ids) == group_sizes[0] for target_ids in chain_target_list.values())
    assert len(chain_target_list) == num_nodes * num_disks_per_node * num_targets_per_disk // group_sizes[0]

  return [Chain(chain_id, target_list) for chain_id, target_list in sorted(chain_target_list.items())]


def main():
  parser = argparse.ArgumentParser(prog="model.py", description="Generate 3FS create target commands")
  parser.add_argument("-type", "--chain_table_type",  type=str, required=True, choices=["EC", "CR"], help="CR - Chain Replication; EC - Erasure Coding")
  parser.add_argument("-b", "--node_id_begin", type=int, required=True, help="The first node id")
  parser.add_argument("-e", "--node_id_end", type=int, required=True, help="The last node id")
  parser.add_argument("-d", "--num_disks_per_node", type=int, required=True, help="Number of disk on each storage node")
  parser.add_argument("-r", "--num_targets_per_disk", type=int, required=True, help="Number of storage targets on each disk")
  parser.add_argument("-tp", "--target_id_prefix", type=int, default=10, help="Prefix of generated target id")
  parser.add_argument("-cp", "--chain_id_prefix", type=int, default=10, help="Prefix of generated chain id")
  parser.add_argument("-cs", "--chunk_size",  nargs="+", help="A list of supported file chunk sizes")
  parser.add_argument("-mat", "--incidence_matrix_path",  type=str, required=True, help="Incidence matrix generated by data placement model")
  parser.add_argument("-o", "--output_path", default="output", help="Path of output files")
  args = parser.parse_args()

  with open(args.incidence_matrix_path, "rb") as fin:
    incidence_matrix = pickle.load(fin)

  assert len(incidence_matrix) < 1_00_000
  assert args.node_id_end - args.node_id_begin < 1000
  assert args.node_id_end < 1_000_000
  assert args.node_id_begin < 1_000_000
  assert args.num_disks_per_node < 1000
  assert args.num_targets_per_disk < 100
  assert args.target_id_prefix < 100
  assert args.chain_id_prefix < 100

  chain_list = generate_chains(**vars(args), incidence_matrix=incidence_matrix)

  with open(os.path.join(args.output_path, "generated_chains.csv"), "w") as fout:
    print(f"ChainId,{','.join(['TargetId']*len(chain_list[0].target_list))}", file=fout)
    for chain in chain_list:
      print(f"{chain.chain_id},{','.join(str(target.target_id) for target in chain.target_list)}", file=fout)

  with open(os.path.join(args.output_path, "generated_chain_table.csv"), "w") as fout:
    print("ChainId", file=fout)
    for chain in chain_list:
      print(f"{chain.chain_id}", file=fout)

  with open(os.path.join(args.output_path, "create_target_cmd.txt"), "w") as fout:
    chunk_size_opt = f"--chunk-size {' '.join(args.chunk_size)}" if args.chunk_size else ""
    for chain in chain_list:
      for target in chain.target_list:
        print(f"create-target --node-id {target.node_id} --disk-index {target.disk_index} --target-id {target.target_id} --chain-id {chain.chain_id} {chunk_size_opt} --use-new-chunk-engine", file=fout)

  with open(os.path.join(args.output_path, "remove_target_cmd.txt"), "w") as fout:
    for chain in chain_list:
      for target in chain.target_list:
        print(f"offline-target --node-id {target.node_id} --target-id {target.target_id}", file=fout)
        print(f"remove-target --node-id {target.node_id} --target-id {target.target_id}", file=fout)


if __name__ == "__main__":
  main()
