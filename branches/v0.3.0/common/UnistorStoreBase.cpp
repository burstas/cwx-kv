#include "UnistorStoreBase.h"
UNISTOR_KEY_GROUP_FN UnistorStoreBase::m_fnKeyGroup=NULL;
UNISTOR_KEY_ASCII_LESS  UnistorStoreBase::m_fnKeyAsciiLess=NULL;

//���������ļ�.-1:failure, 0:success
int UnistorStoreBase::init(UNISTOR_MSG_CHANNEL_FN msgPipeFunc,
                           void* msgPipeApp,
                           UnistorConfig const* config)
{
    m_pMsgPipeFunc =  msgPipeFunc;
    m_pMsgPipeApp = msgPipeApp;
    if (0 != close()) return -1;
    m_config = config;
    m_uiDefExpire = m_config->getCommon().m_uiDefExpire;
    //����binlog
    CWX_UINT64 ullBinLogSize = m_config->getBinlog().m_uiBinLogMSize;
    ullBinLogSize *= 1024 * 1024;

    m_binLogMgr = new CwxBinLogMgr(m_config->getBinlog().m_strBinlogPath.c_str(),
        m_config->getBinlog().m_strBinlogPrex.c_str(),
        ullBinLogSize,
        m_config->getBinlog().m_uiFlushNum,
        m_config->getBinlog().m_uiFlushSecond,
        m_config->getBinlog().m_bDelOutdayLogFile);
    if (0 != m_binLogMgr->init(m_config->getBinlog().m_uiMgrFileNum,
        m_config->getBinlog().m_bCache, m_szErrMsg))
    {
        m_bValid = false;
        CWX_ERROR(("Failure to init binlog, errno=%s", m_szErrMsg));
        return -1;
    }
    if (m_cache) delete (m_cache);
    m_cache = NULL;
    return 0;
}

///����cache
int UnistorStoreBase::startCache(CWX_UINT32 uiWriteCacheMBtye, ///<write cache�Ĵ�С����Ϊ0��ʾû��write cache
               CWX_UINT32 uiReadCacheMByte, ///<��cache�Ĵ�С����Ϊ0��ʾû��read cache
               CWX_UINT32 uiReadMaxCacheKeyNum, ///<��cache�����cache��Ŀ
               UNISTOR_WRITE_CACHE_WRITE_BEGIN_FN fnBeginWrite, ///<д�Ŀ�ʼ����
               UNISTOR_WRITE_CACHE_WRITE_WRITE_FN   fnWrite, ///<д����
               UNISTOR_WRITE_CACHE_WRITE_END_FN     fnEndWrite, ///<д��������
               void*     context, ///<������context
               UNISTOR_KEY_CMP_EQUAL_FN  fnEqual,
               UNISTOR_KEY_CMP_LESS_FN   fnLess,
               UNISTOR_KEY_HASH_FN       fnHash,
               CWX_UINT16     unAlign
               )
{
    if (m_cache) delete (m_cache);
    m_cache = NULL;
    //��ʼ��cache
    m_cache = new UnistorCache(uiWriteCacheMBtye, uiReadCacheMByte, uiReadMaxCacheKeyNum, fnEqual, fnLess, fnHash);
    if (0 != m_cache->init(fnBeginWrite, fnWrite, fnEndWrite, context, unAlign)){
        CWX_ERROR(("Failure to init UnistorCache."));
        return -1;
    }
    return 0;
}

///��ȡ���ݡ�-1��ʧ�ܣ�0��������1����ȡһ��
int UnistorStoreBase::nextCache(UnistorStoreCursor& cursor, char* szErr2K){
    int ret = 0;
    bool bDel = false;
    CWX_UINT16 unKeyLen = 0;
    do{
        do{
            if (cursor.m_bBegin && cursor.m_begin&&cursor.m_bCacheFirst){///�����ȡ��һ��
                cursor.m_bCacheFirst = false;
                cursor.m_uiCacheDataLen = UNISTOR_MAX_KV_SIZE;
                ret = getCache()->getWriteKey(cursor.m_begin,
                    strlen(cursor.m_begin),
                    cursor.m_szCacheData,
                    cursor.m_uiCacheDataLen,
                    bDel);
                if (1 == ret){
                    ///����key
                    cursor.m_unCacheKeyLen = strlen(cursor.m_begin); 
                    memcpy(cursor.m_szCacheKey, cursor.m_begin, cursor.m_unCacheKeyLen);
                    cursor.m_szCacheKey[cursor.m_unCacheKeyLen] = 0x00;
                    break;
                }else if (-1 == ret){
                    if (szErr2K) CwxCommon::snprintf(szErr2K, 2048, "Buf size[%u] is too small.", UNISTOR_MAX_KV_SIZE);
                    return -1;
                }
                cursor.m_unCacheKeyLen = strlen(cursor.m_begin);
                memcpy(cursor.m_szCacheKey, cursor.m_begin, cursor.m_unCacheKeyLen);
            }else if(cursor.m_bCacheFirst){
                cursor.m_bCacheFirst = false;
                if (cursor.m_begin){
                    cursor.m_unCacheKeyLen = strlen(cursor.m_begin);
                    memcpy(cursor.m_szCacheKey, cursor.m_begin, cursor.m_unCacheKeyLen);
                }else{
                    cursor.m_szCacheKey[0]=0x00;
                    cursor.m_unCacheKeyLen = 0;
                }
            }
            ///��ʱ�������ǵ�һ�λ�ȡ����ǰkey�����ڻ���ȡ��һ��
            unKeyLen = UNISTOR_MAX_KEY_SIZE;
            cursor.m_uiCacheDataLen = UNISTOR_MAX_KV_SIZE;
            ret = getCache()->nextWriteKey(cursor.m_szCacheKey,
                cursor.m_unCacheKeyLen,
                cursor.m_asc,
                cursor.m_szCacheKey,
                unKeyLen,
                cursor.m_szCacheData,
                cursor.m_uiCacheDataLen,
                bDel);
            if (0 == ret) return 0;
            if (-1 == ret){
                if (szErr2K) CwxCommon::snprintf(szErr2K, 2048, "Buf size[%u] is too small.", cursor.m_uiCacheDataLen);
                return -1;
            }
            cursor.m_unCacheKeyLen = unKeyLen;
            break;
        }while(1);
        cursor.m_bCacheDel = bDel;
        if (cursor.m_end){
            int ret = m_fnKeyAsciiLess(cursor.m_szCacheKey, cursor.m_unCacheKeyLen, cursor.m_end, strlen(cursor.m_end));
            if (0 == ret) return 0;
            if (cursor.m_asc){
                if (0<ret) return 0;
            }else{
                if (0>ret) return 0;
            }
        }
        return 1;
    }while(1);
    return 1;
}

///�رգ�0���ɹ���-1��ʧ��
int UnistorStoreBase::close(){
    if (m_binLogMgr){
        delete m_binLogMgr;
        m_binLogMgr = NULL;
    }
    m_config = NULL; ///<ϵͳ����
    m_bValid = false;
    strcpy(m_szErrMsg, "No init.");
    return 0;
}

///��binlog�лָ����ݡ�0���ɹ���-1��ʧ��
int UnistorStoreBase::restore(CWX_UINT64 ullSid){
    CWX_ASSERT(m_bValid);
    int ret = 0;
    CwxBinLogCursor* cursor = getBinLogMgr()->createCurser(ullSid);
    UnistorTss* pTss = new UnistorTss();
    pTss->init();
    CWX_UINT32 uiDataLen=0;
    char* pBuf = NULL;
    bool bSeek = false;

    CwxKeyValueItem item;
    CwxKeyValueItem const* pItem = NULL;
    CWX_UINT32 uiType = 0;
    CWX_UINT32 uiVersion = 0;
    CWX_UINT32 uiRestoreNum = 0;

    CWX_INFO(("Begin restore store from binlog, start sid=%s", CwxCommon::toString(ullSid, pTss->m_szBuf2K, 10)));
    while(true){
        if (bSeek){
            ret = getBinLogMgr()->next(cursor);
        }else{
            ret = getBinLogMgr()->seek(cursor, ullSid);
            bSeek = true;
        }
        if (-1 == ret){
            m_bValid = false;
            strcpy(m_szErrMsg, cursor->getErrMsg());
            CWX_ERROR(("Failure to seek cursor, err=%s", m_szErrMsg));
            break;
        }else if (0 == ret){
            break;
        }

        //fetch
        uiDataLen = cursor->getHeader().getLogLen();
        ///׼��data��ȡ��buf
        pBuf = getBuf(uiDataLen);        
        ///��ȡdata
        ret = getBinLogMgr()->fetch(cursor, pBuf, uiDataLen);
        if (-1 == ret){//��ȡʧ��
            m_bValid = false;
            strcpy(m_szErrMsg, cursor->getErrMsg());
            CWX_ERROR(("Failure to fetch data, err:%s", m_szErrMsg));
            break;
        }
        if (!pTss->m_pEngineReader->unpack(pBuf, uiDataLen, false, true))
        {
            CWX_ERROR(("Failure to parase binlog ,err:%s", pTss->m_pEngineReader->getErrMsg()));
            continue;
        }
        //get data
        if (!(pItem = pTss->m_pEngineReader->getKey(UNISTOR_KEY_D)))
        {
            CWX_ERROR(("Can't find key[%s] in binlog", UNISTOR_KEY_D));
            continue;
        }
        item.m_szData = pItem->m_szData;
        item.m_uiDataLen = pItem->m_uiDataLen;
        item.m_bKeyValue = pItem->m_bKeyValue;
        //get type
        if (!pTss->m_pEngineReader->getKey(UNISTOR_KEY_TYPE, uiType)){
            CWX_ERROR(("Can't find key[%s] in binlog", UNISTOR_KEY_TYPE));
            continue;
        }
        //get version
        if (!pTss->m_pEngineReader->getKey(UNISTOR_KEY_V, uiVersion)){
            CWX_ERROR(("Can't find key[%s] in binlog", UNISTOR_KEY_V));
            continue;
        }
        if (0 != syncMasterBinlog(pTss,
            pTss->m_pReader,
            cursor->getHeader().getSid(),
            cursor->getHeader().getDatetime(),
            cursor->getHeader().getGroup(),
            uiType,
            item,
            uiVersion,
            true))
        {
            CWX_ERROR(("Failure to restore binlog , type=%u  err=%s", uiType, pTss->m_szBuf2K));
        }
        uiRestoreNum++;
        if (uiRestoreNum % 10000 == 0){
            CWX_INFO(("Starting check point for restore record:%u ......", uiRestoreNum));
            checkpoint();
            CWX_INFO(("End check point............"));
            ///�������̸����ź�
            CWX_INFO(("send heatbeat to parent proc:%d", ::getppid()));
            if (1 == ::getppid()){
                CWX_INFO(("Exit for no parent."));
                break;
            }
            ::kill(::getppid(), SIGHUP);
        }
    }

    if (!m_bValid){
        strcpy(m_szErrMsg, pTss->m_szBuf2K);
        CWX_ERROR((m_szErrMsg));
    }else{
        CWX_INFO(("End restore store from binlog, end sid=%s", CwxCommon::toString(cursor->getHeader().getSid()==0?ullSid:cursor->getHeader().getSid(), pTss->m_szBuf2K, 10)));
    }
    delete pTss;
    getBinLogMgr()->destoryCurser(cursor);
    return m_bValid?0:-1;
}

///ͬ��master��binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::syncMasterBinlog(UnistorTss* tss,
                                       CwxPackageReader* reader,
                                       CWX_UINT64 ullSid,
                                       CWX_UINT32 ttTimestamp,
                                       CWX_UINT32 uiGroup,
                                       CWX_UINT32 uiType,
                                       CwxKeyValueItem const& value,
                                       CWX_UINT32 uiVersion,
                                       bool bRestore)
{
    int ret = 0;
    CwxKeyValueItem const* key=NULL;
    CwxKeyValueItem const* field = NULL;
    CwxKeyValueItem const* extra = NULL;
    CWX_UINT32		 uiExpire = 0;
    CWX_INT64        llMax = 0;
    CWX_INT64        llMin = 0;
    CWX_INT64        llValue = 0;
    CwxKeyValueItem const* data = NULL;
    CWX_UINT32       uiSign=0;
    char const*      szUser=NULL;
    char const*      szPasswd=NULL;
    CWX_INT64		 num=0;
    CWX_UINT32       uiOldVersion=0;
    bool             bCache=true;
    if (!m_bValid){
        strcpy(tss->m_szBuf2K, m_szErrMsg);
        return -1;
    }
    bool bRet = reader->unpack(value.m_szData, value.m_uiDataLen, false);
    if (!bRet){
        strcpy(tss->m_szBuf2K, reader->getErrMsg());
        return -1;
    }
    switch(uiType){
    case UnistorPoco::MSG_TYPE_RECV_ADD:
        ret = UnistorPoco::parseRecvAdd(reader,
            key,
            field,
            extra,
            data,
            uiExpire,
            uiSign,
            uiOldVersion,
            bCache,
            szUser,
            szPasswd,
            tss->m_szBuf2K);
        if (UNISTOR_ERR_SUCCESS != ret){
            ret = -1;
            break;
        }
        ret = syncAddKey(tss,
            *key,
            field,
            extra,
            *data,
            uiSign,
            uiVersion,
            bCache,
            uiExpire,
            ullSid,
            bRestore);
        if (1 == ret){
            ret = 0;
        }else if(0 == ret){
            CwxCommon::snprintf(tss->m_szBuf2K, 2047, "Not find key [%s]", key->m_szData);
            ret = -1;
        }else{
            ret = -1;
        }
        break;
    case UnistorPoco::MSG_TYPE_RECV_SET:
        ret = UnistorPoco::parseRecvSet(reader,
            key,
            field,
            extra,
            data,
            uiSign,
            uiExpire,
            uiOldVersion,
            bCache,
            szUser,
            szPasswd,
            tss->m_szBuf2K);
        if (UNISTOR_ERR_SUCCESS != ret){
            ret = -1;
            break;
        }
        ret = syncSetKey(tss,
            *key,
            field,
            extra,
            *data,
            uiSign,
            uiVersion,
            bCache,
            uiExpire,
            ullSid,
            bRestore);
        if (1 == ret){
            ret = 0;
        }else if(0 == ret){
            CwxCommon::snprintf(tss->m_szBuf2K, 2047, "Not find key [%s]", key->m_szData);
            ret = -1;
        }else{
            ret = -1;
        }
        break;
    case UnistorPoco::MSG_TYPE_RECV_UPDATE:
        ret = UnistorPoco::parseRecvUpdate(reader,
            key,
            field,
            extra,
            data,
            uiSign,
            uiExpire,
            uiOldVersion,
            szUser,
            szPasswd,
            tss->m_szBuf2K);
        if (UNISTOR_ERR_SUCCESS != ret){
            ret = -1;
            break;
        }
        ret = syncUpdateKey(tss,
            *key,
            field,
            extra,
            *data,
            uiSign,
            uiVersion,
            uiExpire,
            ullSid,
            bRestore); ///����������
        if (1 == ret){
            ret = 0;
        }else if(0 == ret){
            CwxCommon::snprintf(tss->m_szBuf2K, 2047, "Not find key [%s]", key->m_szData);
            ret = -1;
        }else{
            ret = -1;
        }
        break;
    case UnistorPoco::MSG_TYPE_RECV_INC:
        ret = UnistorPoco::parseRecvInc(reader,
            key,
            field,
            extra,
            num,
            llMax,
            llMin,
            uiExpire,
            uiSign,
            uiOldVersion,
            szUser,
            szPasswd,
            tss->m_szBuf2K);
        if (UNISTOR_ERR_SUCCESS != ret){
            ret = -1;
            break;
        }
        ret = syncIncKey(tss,
            *key,
            field,
            extra,
            num,
            0,
            0,
            uiSign,
            llValue,
            uiVersion,
            uiExpire,
            ullSid,
            bRestore); ///����������
        if (1 == ret){
            ret = 0;
        }else if(0 == ret){
            CwxCommon::snprintf(tss->m_szBuf2K, 2047, "Not find key [%s]", key->m_szData);
            ret = -1;
        }else{
            ret = -1;
        }
        break;
    case UnistorPoco::MSG_TYPE_RECV_DEL:
        ret = UnistorPoco::parseRecvDel(reader,
            key,
            field,
            extra,
            uiOldVersion,
            szUser,
            szPasswd,
            tss->m_szBuf2K);
        if (UNISTOR_ERR_SUCCESS != ret){
            ret = -1;
            break;
        }
        ret = syncDelKey(tss,
            *key,
            field,
            extra,
            uiVersion,
            ullSid,
            bRestore); ///������ɾ��
        if (1 == ret){
            ret = 0;
        }else if(0 == ret){
            CwxCommon::snprintf(tss->m_szBuf2K, 2047, "Not find key [%s]", key->m_szData);
            ret = -1;
        }else{
            ret = -1;
        }
        break;
    case UnistorPoco::MSG_TYPE_TIMESTAMP:
        m_ttExpireClock = ttTimestamp; ///<���ó�ʱ��clock
        ret = 0;
        break;
    default:
        ret = -1;
        CwxCommon::snprintf(tss->m_szBuf2K, 2047, "Unknown binlog type:%d", uiType);
        break;
    }
    ///��������дbinlog
    if (!bRestore){
        tss->m_pEngineWriter->beginPack();
        if (!tss->m_pEngineWriter->addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), value.m_szData, value.m_uiDataLen, true)){
            strcpy(tss->m_szBuf2K, tss->m_pEngineWriter->getErrMsg());
            return -1;
        }
        if (!tss->m_pEngineWriter->addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
            strcpy(tss->m_szBuf2K, tss->m_pEngineWriter->getErrMsg());
            return -1;
        }
        if (!tss->m_pEngineWriter->addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
            strcpy(tss->m_szBuf2K, tss->m_pEngineWriter->getErrMsg());
            return -1;
        }
        tss->m_pEngineWriter->pack();
        if (0 != appendBinlog(ullSid,
            ttTimestamp,
            uiGroup,
            tss->m_pEngineWriter->getMsg(),
            tss->m_pEngineWriter->getMsgSize(),
            tss->m_szBuf2K))
        {
            m_bValid = false;
            return -1;
        }
    }
    return ret;
}

///���Add key binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::appendTimeStampBinlog(CwxPackageWriter& writer,
                                      CWX_UINT32 ttNow,
                                      char* szErr2K)
{
    writer.beginPack();
    if (!writer.addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), "", 0, false))
    {
        if (szErr2K) strcpy(szErr2K, writer.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiType = UnistorPoco::MSG_TYPE_TIMESTAMP;
    if (!writer.addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
        if (szErr2K) strcpy(szErr2K, writer.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiVersion = 0;
    if (!writer.addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
        if (szErr2K) strcpy(szErr2K, writer.getErrMsg());
        return -1;
    }
    writer.pack();
    CWX_UINT64 ullSid = 0;
    int ret = appendBinlog(ullSid, ttNow, 0, writer.getMsg(), writer.getMsgSize(), szErr2K);
    if (0 == ret){
        m_ullStoreSid = ullSid;
    }
    return ret;
}


///���Add key binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::appendAddBinlog(CwxPackageWriter& writer,
                                      CwxPackageWriter& writer1,
                                      CWX_UINT32 uiGroup,
                                      CwxKeyValueItem const& key,
                                      CwxKeyValueItem const* field,
                                      CwxKeyValueItem const* extra,
                                      CwxKeyValueItem const& data,
                                      CWX_UINT32    uiExpire,
                                      CWX_UINT32    uiSign,
                                      CWX_UINT32    uiVersion,
                                      bool          bCache,
                                      char*			szErr2K)
{
    writer.beginPack();
    if (UNISTOR_ERR_SUCCESS != UnistorPoco::packRecvAdd(&writer,
        key,
        field,
        extra,
        data,
        uiExpire,
        uiSign,
        0,
        bCache,
        NULL,
        NULL,
        szErr2K))
    {
        return -1;
    }
    writer.pack();
    writer1.beginPack();
    if (!writer1.addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), writer.getMsg(), writer.getMsgSize(), true))
    {
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiType = UnistorPoco::MSG_TYPE_RECV_ADD;
    if (!writer1.addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    if (!writer1.addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    writer1.pack();
    CWX_UINT64 ullSid = 0;
    return appendBinlog(ullSid, time(NULL), uiGroup, writer1.getMsg(), writer1.getMsgSize(), szErr2K);
}

///���set key binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::appendSetBinlog(CwxPackageWriter& writer,
                                      CwxPackageWriter& writer1,
                                      CWX_UINT32 uiGroup,
                                      CwxKeyValueItem const& key,
                                      CwxKeyValueItem const* field,
                                      CwxKeyValueItem const* extra,
                                      CwxKeyValueItem const& data,
                                      CWX_UINT32    uiExpire,
                                      CWX_UINT32    uiSign,
                                      CWX_UINT32    uiVersion,
                                      bool          bCache,
                                      char*			 szErr2K)
{
    writer.beginPack();
    if (UNISTOR_ERR_SUCCESS != UnistorPoco::packRecvSet(&writer,
        key,
        field,
        extra,
        data,
        uiSign,
        uiExpire,
        0,
        bCache,
        NULL,
        NULL,
        szErr2K))
    {
        return -1;
    }
    writer.pack();
    writer1.beginPack();
    if (!writer1.addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), writer.getMsg(), writer.getMsgSize(), true))
    {
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiType = UnistorPoco::MSG_TYPE_RECV_SET;
    if (!writer1.addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    if (!writer1.addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    writer1.pack();
    CWX_UINT64 ullSid = 0;
    return appendBinlog(ullSid, time(NULL), uiGroup, writer1.getMsg(), writer1.getMsgSize(), szErr2K);
}

///���update key binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::appendUpdateBinlog(CwxPackageWriter& writer,
                                         CwxPackageWriter& writer1,
                                         CWX_UINT32 uiGroup,
                                         CwxKeyValueItem const& key,
                                         CwxKeyValueItem const* field,
                                         CwxKeyValueItem const* extra,
                                         CwxKeyValueItem const& data,
                                         CWX_UINT32       uiExpire,
                                         CWX_UINT32 uiSign,
                                         CWX_UINT32       uiVersion,
                                         char*			  szErr2K)
{
    writer.beginPack();
    if (UNISTOR_ERR_SUCCESS != UnistorPoco::packRecvUpdate(&writer,
        key,
        field,
        extra,
        data,
        uiSign,
        uiExpire))
    {
        return -1;
    }
    writer.pack();
    writer1.beginPack();
    if (!writer1.addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), writer.getMsg(), writer.getMsgSize(), true))
    {
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiType = UnistorPoco::MSG_TYPE_RECV_UPDATE;
    if (!writer1.addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    if (!writer1.addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    writer1.pack();
    CWX_UINT64 ullSid = 0;
    return appendBinlog(ullSid, time(NULL), uiGroup, writer1.getMsg(), writer1.getMsgSize(), szErr2K);
}

///���inc key binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::appendIncBinlog(CwxPackageWriter& writer,
                                      CwxPackageWriter& writer1,
                                      CWX_UINT32 uiGroup,
                                      CwxKeyValueItem const& key,
                                      CwxKeyValueItem const* field,
                                      CwxKeyValueItem const* extra,
                                      int	             num,
                                      CWX_INT64        llMax,
                                      CWX_INT64        llMin,
                                      CWX_UINT32    uiExpire,
                                      CWX_UINT32       uiSign,
                                      CWX_UINT32       uiVersion,
                                      char*			 szErr2K)
{
    writer.beginPack();
    if (UNISTOR_ERR_SUCCESS != UnistorPoco::packRecvInc(&writer,
        key,
        field,
        extra,
        num,
        llMax,
        llMin,
        uiExpire,
        uiSign))
    {
        return -1;
    }
    writer.pack();
    writer1.beginPack();
    if (!writer1.addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), writer.getMsg(), writer.getMsgSize(), true))
    {
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiType = UnistorPoco::MSG_TYPE_RECV_INC;
    if (!writer1.addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    if (!writer1.addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    writer1.pack();
    CWX_UINT64 ullSid = 0;
    return appendBinlog(ullSid, time(NULL),
        uiGroup,
        writer1.getMsg(),
        writer1.getMsgSize(),
        szErr2K);
}

///���del key binlog.0���ɹ���-1��ʧ��
int UnistorStoreBase::appendDelBinlog(CwxPackageWriter& writer,
                                      CwxPackageWriter& writer1,
                                      CWX_UINT32        uiGroup,
                                      CwxKeyValueItem const& key,
                                      CwxKeyValueItem const* field,
                                      CwxKeyValueItem const* extra,
                                      CWX_UINT32       uiVersion,
                                      char*			 szErr2K)
{
    writer.beginPack();
    if (UNISTOR_ERR_SUCCESS != UnistorPoco::packRecvDel(&writer,
        key,
        field,
        extra))
    {
        return -1;
    }
    writer.pack();
    writer1.beginPack();
    if (!writer1.addKeyValue(UNISTOR_KEY_D, strlen(UNISTOR_KEY_D), writer.getMsg(), writer.getMsgSize(), true))
    {
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    CWX_UINT32 uiType = UnistorPoco::MSG_TYPE_RECV_DEL;
    if (!writer1.addKeyValue(UNISTOR_KEY_TYPE, strlen(UNISTOR_KEY_TYPE), uiType)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    if (!writer1.addKeyValue(UNISTOR_KEY_V, strlen(UNISTOR_KEY_V), uiVersion)){
        if (szErr2K) strcpy(szErr2K, writer1.getErrMsg());
        return -1;
    }
    writer1.pack();
    CWX_UINT64 ullSid = 0;
    return appendBinlog(ullSid, time(NULL), uiGroup, writer1.getMsg(), writer1.getMsgSize(), szErr2K);
}


///������[\n]�ָ��Ķ���ֶ�
int UnistorStoreBase::parseMultiField(char const* szValue, UnistorKeyField*& field){
    int num = 0;
    if (szValue){
        UnistorKeyField* field_tmp = NULL;
        list<string> fields;
        CwxCommon::split(string(szValue), fields, '\n');
        list<string>::iterator iter = fields.begin();
        while(iter != fields.end()){
            CwxCommon::trim(*iter);
            if (iter->length()){
                if (!field_tmp){
                    num++;
                    field_tmp = field = new UnistorKeyField();
                }else{
                    num++;
                    field_tmp->m_next = new UnistorKeyField();
                    field_tmp = field_tmp->m_next;
                }
                field_tmp->m_key = *iter;
            }
            iter++;
        }
    }
    return num;
}


///�ͷ�key
void UnistorStoreBase::freeField(UnistorKeyField*& field){
    UnistorKeyField* next;
    while(field){
        next = field->m_next;
        delete field;
        field = next;
    }
    field = NULL;
}

///��add key���¡���field���й鲢��-1��ʧ�ܣ�0�����ڣ�1���ɹ�
int UnistorStoreBase::mergeAddKeyField(CwxPackageWriter* writer1,
                                       CwxPackageReader* reader1,
                                       CwxPackageReader* reader2,
                                       char const* key,
                                       CwxKeyValueItem const* field,
                                       char const* szOldData,
                                       CWX_UINT32 uiOldDataLen,
                                       bool bOldKeyValue,
                                       char const* szNewData,
                                       CWX_UINT32 uiNewDataLen,
                                       bool bNewKeyValue,
                                       CWX_UINT32& uiFieldNum,
                                       char* szErr2K)
{
    CWX_UINT32 i=0;
    CwxKeyValueItem const* pItem=NULL;
    ///��ֵһ����kv�ṹ
    CWX_ASSERT(bOldKeyValue);
    uiFieldNum = 0;
    if (!reader1->unpack(szOldData, uiOldDataLen, false, true)){
        strcpy(szErr2K, reader1->getErrMsg());
        return -1;
    }
    if (field){
        writer1->beginPack();
        for (i=0; i<reader1->getKeyNum(); i++){
            pItem = reader1->getKey(i);
            if ((pItem->m_unKeyLen == field->m_uiDataLen) && (memcmp(pItem->m_szKey, field->m_szData, pItem->m_unKeyLen)==0)){
                CwxCommon::snprintf(szErr2K, 2047, "Key[%s]'s field [%s] exist.", key, field->m_szData);
                return 0;
            }
            if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen,  pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
        }
        if (!writer1->addKeyValue(field->m_szData, field->m_uiDataLen, szNewData, uiNewDataLen, bNewKeyValue)){
            strcpy(szErr2K, writer1->getErrMsg());
            return -1;
        }
        writer1->pack();
        uiFieldNum = reader1->getKeyNum() + 1;
        return 1;
    }
    CWX_ASSERT(bOldKeyValue);
    if (!reader2->unpack(szNewData, uiNewDataLen)){
        strcpy(szErr2K, reader2->getErrMsg());
        return -1;
    }
    writer1->beginPack();
    for (i=0; i<reader1->getKeyNum(); i++){
        pItem = reader1->getKey(i);
        if (reader2->getKey(pItem->m_szKey)){
            CwxCommon::snprintf(szErr2K, 2047, "Key[%s]'s field [%s] exist.", key, pItem->m_szKey);
            return 0;
        }
        if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
            strcpy(szErr2K, writer1->getErrMsg());
            return -1;
        }
    }
    for (i=0; i<reader2->getKeyNum(); i++){
        pItem = reader2->getKey(i);
        if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
            strcpy(szErr2K, writer1->getErrMsg());
            return -1;
        }
    }
    writer1->pack();
    uiFieldNum = reader1->getKeyNum() + reader2->getKeyNum();
    return 1;
}

int UnistorStoreBase::mergeSetKeyField(CwxPackageWriter* writer1,
                                       CwxPackageReader* reader1,
                                       CwxPackageReader* reader2,
                                       char const* ,
                                       CwxKeyValueItem const* field,
                                       char const* szOldData,
                                       CWX_UINT32 uiOldDataLen,
                                       bool ,
                                       char const* szNewData,
                                       CWX_UINT32 uiNewDataLen,
                                       bool bNewKeyValue,
                                       CWX_UINT32& uiFieldNum,
                                       char* szErr2K)
{
    CWX_UINT32 i=0;
    CwxKeyValueItem const* pItem=NULL;
    if (!reader1->unpack(szOldData, uiOldDataLen, false, true)){
        strcpy(szErr2K, reader1->getErrMsg());
        return -1;
    }
    uiFieldNum = 0;
    if (field){///setһ����
        bool bAddField = false;
        writer1->beginPack();
        for (i=0; i<reader1->getKeyNum(); i++){
            pItem = reader1->getKey(i);
            if ((pItem->m_unKeyLen == field->m_uiDataLen) && (memcmp(pItem->m_szKey, field->m_szData, pItem->m_unKeyLen)==0)){
                if (!writer1->addKeyValue(field->m_szData, field->m_uiDataLen, szNewData, uiNewDataLen, bNewKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                bAddField = true;
                uiFieldNum++;
            }else{
                if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                uiFieldNum++;
            }
        }
        if (!bAddField){///
            if (!writer1->addKeyValue(field->m_szData, field->m_uiDataLen, szNewData, uiNewDataLen, bNewKeyValue)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
            uiFieldNum++;
        }
        writer1->pack();
        return 1;
    }else{
        if (!reader2->unpack(szNewData, uiNewDataLen)){
            strcpy(szErr2K, reader2->getErrMsg());
            return -1;
        }
        writer1->beginPack();
        for (i=0; i<reader2->getKeyNum(); i++){
            pItem = reader2->getKey(i);
            if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
            uiFieldNum++;
        }
        for (i=0; i<reader1->getKeyNum(); i++){
            pItem = reader1->getKey(i);
            if (!reader2->getKey(pItem->m_szKey)){
                if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                uiFieldNum++;
            }
        }
        writer1->pack();
    }
    return 1;
}

///��update key���¡���field���й鲢��-1��ʧ�ܣ�0�������ڣ�1���ɹ�
int UnistorStoreBase::mergeUpdateKeyField(CwxPackageWriter* writer1,
                                          CwxPackageReader* reader1,
                                          CwxPackageReader* reader2,
                                          char const* key,
                                          CwxKeyValueItem const* field,
                                          char const* szOldData,
                                          CWX_UINT32 uiOldDataLen,
                                          bool bOldKeyValue,
                                          char const* szNewData,
                                          CWX_UINT32 uiNewDataLen,
                                          bool bNewKeyValue,
                                          CWX_UINT32& uiFieldNum,
                                          bool bAppend,
                                          char* szErr2K)
{
    CWX_UINT32 i=0;
    CwxKeyValueItem const* pItem=NULL;
    ///��ֵһ����kv�ṹ
    CWX_ASSERT(bOldKeyValue);
    if (!reader1->unpack(szOldData, uiOldDataLen, false, true)){
        strcpy(szErr2K, reader1->getErrMsg());
        return -1;
    }
    uiFieldNum = 0;
    if (field){///update��key
        bool bAdd=false;
        if (!reader1->getKey(field->m_szData) && !bAppend){
            CwxCommon::snprintf(szErr2K, 2047, "Key[%s]'s field [%s] doesn't exist.", key, field->m_szData);
            return 0;
        }
        writer1->beginPack();
        for (i=0; i<reader1->getKeyNum(); i++){
            pItem = reader1->getKey(i);
            if ((pItem->m_unKeyLen != field->m_uiDataLen) || (memcmp(pItem->m_szKey, field->m_szData, pItem->m_unKeyLen)!=0)){
                if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                bAdd = true;
                uiFieldNum++;
            }else{
                if (!writer1->addKeyValue(field->m_szData, field->m_uiDataLen, szNewData, uiNewDataLen, bNewKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                uiFieldNum++;
            }
        }
        if (!bAdd){
            if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
            uiFieldNum++;
        }
        writer1->pack();
        return 1;
    }else{
        if (!reader2->unpack(szNewData, uiNewDataLen)){
            strcpy(szErr2K, reader2->getErrMsg());
            return -1;
        }
        if (!bAppend){
            for (i=0; i<reader2->getKeyNum(); i++){
                if (!reader1->getKey(reader2->getKey(i)->m_szKey)){
                    CwxCommon::snprintf(szErr2K, 2047, "Key[%s]'s field [%s] doesn't exist.", key, reader2->getKey(i)->m_szKey);
                    return 0;
                }
            }
        }
        writer1->beginPack();
        for (i=0; i<reader2->getKeyNum(); i++){
            pItem = reader2->getKey(i);
            if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
            uiFieldNum++;
        }
        for (i=0; i<reader1->getKeyNum(); i++){
            pItem = reader1->getKey(i);
            if (!reader2->getKey(pItem->m_szKey)){
                if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                uiFieldNum++;
            }
        }
        writer1->pack();
    }
    return 1;
}

///��int key�ĸ��¡�-2��key�����߽磻-1��ʧ�ܣ�0�������ڣ�1���ɹ�;
int UnistorStoreBase::mergeIncKeyField(CwxPackageWriter* writer1,
                                       CwxPackageReader* reader1,
                                       char const* ,
                                       CwxKeyValueItem const* field,
                                       char const* szOldData,
                                       CWX_UINT32 uiOldDataLen,
                                       bool bOldKeyValue,
                                       int  num,
                                       CWX_INT64  llMax,
                                       CWX_INT64  llMin,
                                       CWX_INT64& llValue,
                                       char* szBuf,
                                       CWX_UINT32& uiBufLen,
                                       bool& bKeyValue,
                                       CWX_UINT32 uiSign,
                                       char* szErr2K)
{
    CWX_UINT32 i=0;
    CwxKeyValueItem const* pItem=NULL;
    char szValue[64];
    if (bOldKeyValue){//Key�ľ�ֵ����
        if (!field){//���key���Ƕ�����key����Ϊ������
            if (0 == uiSign){///�������������
                strcpy(szErr2K, "Key is not counter.");
                return 0;
            }
            llValue = num;
            if ((num>0) && llMax && (llValue > llMax)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is more than the max[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMax, szBuf, 0));
                return -2;
            }else if ((num<0) && llMin && (llValue < llMin)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is less than the min[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMin, szBuf, 0));
                return -2;
            }
            CwxCommon::toString((CWX_INT64)num, szBuf, 0);
            uiBufLen = strlen(szBuf);
            bKeyValue = false;
            return 1;
        }
        ///ָ����field
        if (!reader1->unpack(szOldData, uiOldDataLen, false, true)){
            strcpy(szErr2K, reader1->getErrMsg());
            return -1;
        }
        if (0 == uiSign){
            if (!reader1->getKey(field->m_szData)){
                strcpy(szErr2K, "Field counter doesn't exist..");
                return 0;
            }
        }
        bool bFindField = false;
        writer1->beginPack();
        for (i=0; i<reader1->getKeyNum(); i++){
            pItem = reader1->getKey(i);
            if ((pItem->m_unKeyLen == field->m_uiDataLen) && (memcmp(pItem->m_szKey, field->m_szData, pItem->m_unKeyLen)==0)){
                llValue = strtoll(pItem->m_szData, NULL, 0);
                llValue += num;
                if ((num>0) && llMax && (llValue > llMax)){
                    CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is more than the max[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMax, szBuf, 0));
                    return -2;
                }else if ((num<0) && llMin && (llValue < llMin)){
                    CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is less than the min[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMin, szBuf, 0));
                    return -2;
                }
                CwxCommon::toString((CWX_INT64)llValue, szBuf, 0);
                if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, szBuf, strlen(szBuf), false)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
                bFindField = true;
            }else{
                if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                    strcpy(szErr2K, writer1->getErrMsg());
                    return -1;
                }
            }
        }
        if (!bFindField){
            llValue = num;
            if ((num>0) && llMax && (llValue > llMax)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is more than the max[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMax, szBuf, 0));
                return -2;
            }else if ((num<0) && llMin && (llValue < llMin)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is less than the min[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMin, szBuf, 0));
                return -2;
            }
            if (!writer1->addKeyValue(field->m_szData, field->m_uiDataLen,  num)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
        }
        writer1->pack();
        uiBufLen = writer1->getMsgSize();
        memcpy(szBuf, writer1->getMsg(), uiBufLen);
        bKeyValue = true;
        return 1;
    }else{//key�ľ�ֵ���Ƕ���
        if (!field){//��ֵҲ���Ƕ���
            if (uiOldDataLen > 63) uiOldDataLen=63;
            memcpy(szValue, szOldData, uiOldDataLen);
            szValue[uiOldDataLen] = 0x00;
            llValue = strtoll(szValue, NULL, 0);
            llValue += num;
            if ((num>0) && llMax && (llValue > llMax)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is more than the max[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMax, szBuf, 0));
                return -2;
            }else if ((num<0) && llMin && (llValue < llMin)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is less than the min[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMin, szBuf, 0));
                return -2;
            }
            CwxCommon::toString((CWX_INT64)llValue, szBuf, 0);
            uiBufLen = strlen(szBuf);
            bKeyValue = false;
            return 1;
        }else{
            if (0 == uiSign){
                strcpy(szErr2K, "Field counter doesn't exist..");
                return 0;
            }
            llValue = num;
            if ((num>0) && llMax && (llValue > llMax)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is more than the max[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMax, szBuf, 0));
                return -2;
            }else if ((num<0) && llMin && (llValue < llMin)){
                CwxCommon::snprintf(szErr2K, 2047, "Couter[%s] is less than the min[%s]", CwxCommon::toString(llValue, szValue, 0), CwxCommon::toString(llMin, szBuf, 0));
                return -2;
            }
            CwxCommon::toString((CWX_INT64)llValue, szBuf, 0);
            writer1->beginPack();
            if (!writer1->addKeyValue(field->m_szData, field->m_uiDataLen, szBuf, strlen(szBuf), false)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
            writer1->pack();
            uiBufLen = writer1->getMsgSize();
            memcpy(szBuf, writer1->getMsg(), uiBufLen);
            bKeyValue = true;
        }
    }
    return 1;
}

///��delete  key��field��-1��ʧ�ܣ�0�������ڣ�1���ɹ�
int UnistorStoreBase::mergeRemoveKeyField(CwxPackageWriter* writer1,
                                          CwxPackageReader* reader1,
                                          char const* key,
                                          CwxKeyValueItem const* field,
                                          char const* szOldData,
                                          CWX_UINT32 uiOldDataLen,
                                          bool ,
                                          bool bSync,
                                          CWX_UINT32& uiFieldNum,
                                          char* szErr2K)
{
    CWX_UINT32 i=0;
    CwxKeyValueItem const* pItem=NULL;
    uiFieldNum = 0;
    if (!reader1->unpack(szOldData, uiOldDataLen, false, true)){
        strcpy(szErr2K, reader1->getErrMsg());
        return -1;
    }
    uiFieldNum = reader1->getKeyNum();
    if (!reader1->getKey(field->m_szData)){
        if (!bSync){
            CwxCommon::snprintf(szErr2K, 2047, "Key[%s]'s field [%s] doesn't exist.", key, field->m_szData);
            return 0;
        }
        return 1;
    }
    writer1->beginPack();
    for (i=0; i<reader1->getKeyNum(); i++){
        pItem = reader1->getKey(i);
        if ((pItem->m_unKeyLen != field->m_uiDataLen) || (memcmp(pItem->m_szKey, field->m_szData, pItem->m_unKeyLen)!=0)){
            if (!writer1->addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue)){
                strcpy(szErr2K, writer1->getErrMsg());
                return -1;
            }
        }else{
            uiFieldNum--;
        }
    }
    writer1->pack();
    return 1;
}

///��ȡָ����field, UNISTOR_ERR_SUCCESS���ɹ����������������
int UnistorStoreBase::pickField(CwxPackageReader& reader,
                                CwxPackageWriter& write,
                                UnistorKeyField const* field,
                                char const* szData,
                                CWX_UINT32 uiDataLen,
                                char* szErr2K)
{
    if (!reader.unpack(szData, uiDataLen, false, true)){
        if (szErr2K) strcpy(szErr2K, reader.getErrMsg());
        return UNISTOR_ERR_ERROR;
    }
    CwxKeyValueItem const* pItem = NULL;
    UnistorKeyField const* field_item = NULL;
    write.beginPack();
    field_item = field;
    while(field_item){
        pItem = reader.getKey(field_item->m_key.c_str());
        if (pItem){
            write.addKeyValue(pItem->m_szKey, pItem->m_unKeyLen, pItem->m_szData, pItem->m_uiDataLen, pItem->m_bKeyValue);
        }
        field_item = field_item->m_next;
    }
    write.pack();
    return UNISTOR_ERR_SUCCESS;
}


///���ĸ�ʽ�Ƿ�Ϸ�
bool UnistorStoreBase::isValidSubscribe(UnistorSubscribe const& , string& ){
    return true;
}
