#include "registerAdminCommands.h"

#include "AdminUserCtrl.h"
#include "Bench.h"
#include "Chdir.h"
#include "Checksum.h"
#include "Create.h"
#include "CreateRange.h"
#include "CreateTarget.h"
#include "CreateTargets.h"
#include "DecodeUserToken.h"
#include "DropUserCache.h"
#include "DumpChainTable.h"
#include "DumpChains.h"
#include "DumpChunkMeta.h"
#include "DumpDirEntries.h"
#include "DumpInodes.h"
#include "FillZero.h"
#include "FindOrphanedChunks.h"
#include "GetConfig.h"
#include "GetLastConfigUpdateRecord.h"
#include "GetRealPath.h"
#include "HotUpdateConfig.h"
#include "InitCluster.h"
#include "List.h"
#include "ListChainTables.h"
#include "ListChains.h"
#include "ListClients.h"
#include "ListNodes.h"
#include "ListTargets.h"
#include "Mkdir.h"
#include "OfflineTarget.h"
#include "OpenRange.h"
#include "ParseTargetMeta.h"
#include "QueryChunk.h"
#include "ReadBench.h"
#include "ReadFile.h"
#include "RefreshRoutingInfo.h"
#include "RegisterNode.h"
#include "RemoteCall.h"
#include "Remove.h"
#include "RemoveChunks.h"
#include "RemoveRange.h"
#include "RemoveTarget.h"
#include "Rename.h"
#include "RenderConfig.h"
#include "RotateAsPreferredOrder.h"
#include "RotateLastSrv.h"
#include "ScanTree.h"
#include "SetConfig.h"
#include "SetLayout.h"
#include "SetNodeTags.h"
#include "SetPermission.h"
#include "SetPreferredTargetOrder.h"
#include "ShutdownAllChains.h"
#include "Stat.h"
#include "Symlink.h"
#include "UnregisterNode.h"
#include "UpdateChain.h"
#include "UploadChainTable.h"
#include "UploadChains.h"
#include "VerifyConfig.h"
#include "WriteFile.h"
#include "client/cli/admin/DumpSession.h"
#include "client/cli/admin/ListGc.h"
#include "client/cli/admin/PruneSession.h"
#include "client/cli/admin/RecursiveChown.h"
#include "common/utils/Result.h"

namespace hf3fs::client::cli {
CoTryTask<void> registerAdminCommands(Dispatcher &dispatcher) {
  CO_RETURN_ON_ERROR(co_await registerAdminUserCtrlHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerInitClusterHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerUploadChainTableHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListChainTablesHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListChainsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDumpChainTableHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDecodeUserTokenHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListNodesHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerSetNodeTagsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRefreshRoutingInfoHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListClientsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRegisterNodesHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerSetConfigHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerGetConfigHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerStatHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerCreateHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerCreateRangeHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRemoveRangeHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerOpenRangeHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerChdirHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerMkdirHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerSymlinkHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRemoveHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRenameHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerGetRealPathHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerUploadChainsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListTargetsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDumpChainsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerSetLayoutHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerCreateTargetHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerCreateTargetsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerOfflineTargetHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRemoveTargetHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerQueryChunkHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerParseTargetMetaHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerChecksumHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerReadBenchHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerReadFileHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerWriteFileHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerFillZeroHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerBenchHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerUnregisterNodesHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRenderConfigHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerHotUpdateConfigHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerGetLastConfigUpdateRecordHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerSetPermissionHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDropUserCacheHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRotateLastSrvHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRotateAsPreferredOrderHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDumpSessionHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerPruneSessionHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerListGcHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRemoteCallHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerShutdownAllChainsHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerVerifyConfigHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerSetPreferredTargetOrderHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerScanTree(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerUpdateChainHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDumpChunkMetaHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDumpInodesHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerDumpDirEntriesHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerFindOrphanedChunksHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRemoveChunksHandler(dispatcher));
  CO_RETURN_ON_ERROR(co_await registerRecursiveChownHandler(dispatcher));
  co_return Void{};
}
}  // namespace hf3fs::client::cli
