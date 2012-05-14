#ifndef __UNISTOR_STORE_BASE_H__
#define __UNISTOR_STORE_BASE_H__

#include "UnistorMacro.h"
#include "UnistorConfig.h"
#include "UnistorTss.h"
#include "CwxBinLogMgr.h"
#include "CwxMd5.h"
#include "UnistorPoco.h"
#include "UnistorCache.h"
#include "UnistorSubscribe.h"

///��������Ϣͨ������
typedef int (*UNISTOR_MSG_CHANNEL_FN)(void* app, ///<app����
                                   CwxMsgBlock* msg, ///<���ݵ���Ϣ
                                   bool bWriteThread, ///<�Ƿ񴫵ݸ�write�̣߳������checkpoint�߳�
                                   char* szErr2K ///<����ʧ��ʱ�Ĵ�����Ϣ
                                   ); 

///key��field���ֶ���
class UnistorKeyField{
public:
    ///���캯��
	UnistorKeyField(){
		m_next = NULL;
	}
public:
	UnistorKeyField*   m_next; ///<��һ��field
	string		       m_key;  ///<field������
};

///expire key�������
struct UnistorStoreExpireKey{
public:
    CWX_UINT32       m_ttExpire; ///<key��expireʱ��
    unsigned char    m_key[0]; ///<key������
}__attribute__ ((__packed__));


///cursor�Ķ���
class UnistorStoreCursor{
public:
    UnistorStoreCursor():m_begin(NULL), m_end(NULL), m_asc(true), m_bBegin(false){
        m_cursorHandle = NULL;
        m_szCacheKey[0] = 0x00;
        m_unCacheKeyLen = 0;
        m_szCacheData[0] = 0x00;
        m_uiCacheDataLen = 0;
        m_bCacheValue = false;
        m_bCacheMore = true;
        m_bCacheFirst = true;
        m_bCacheDel = false;

        m_szStoreKey[0] = 0x00;
        m_unStoreKeyLen = 0;
        m_szStoreData[0] = 0x00;
        m_uiStoreDataLen = 0;
        m_bStoreValue = false;
        m_bStoreMore = true;

    }

    UnistorStoreCursor(char const* begin,
        char const* end,
        bool bAsc,
        bool bBegin):m_begin(begin), m_end(end), m_asc(bAsc), m_bBegin(bBegin)
    {
        m_cursorHandle = NULL;
        m_field = NULL;
        m_bCacheFirst = true;

        m_szCacheKey[0] = 0x00;
        m_unCacheKeyLen = 0;
        m_szCacheData[0] = 0x00;
        m_uiCacheDataLen = 0;
        m_bCacheValue = false;
        m_bCacheMore = true;
        m_bCacheDel = false;

        m_szStoreKey[0] = 0x00;
        m_unStoreKeyLen = 0;
        m_szStoreData[0] = 0x00;
        m_uiStoreDataLen = 0;
        m_bStoreValue = false;
        m_bStoreMore = true;

    }
public:
    char const*       m_begin; ///<��ʼλ�ã���ΪNULL����ʾ��store�Ŀ�ʼλ�ã�asc=true)�����λ��(asc=false)
    char const*       m_end; ///<����λ�ã���ΪNULL���޶�����λ��
    bool const        m_asc; ///<�Ƿ�����
    bool const        m_bBegin; ///<�Ƿ������ʼֵ
    void*             m_cursorHandle; ///<store��handle

    UnistorKeyField*  m_field; ///<field��Ϣ 
    UnistorSubscribe  m_scribe; ///���Ĺ���

    string           m_strBeginKey; ///<export�Ŀ�ʼkey
    string           m_strExtra; ///<export��extra

    char			  m_szCacheKey[UNISTOR_MAX_KEY_SIZE]; ///<cache��ǰ��key
    CWX_UINT16        m_unCacheKeyLen; ///<key�ĳ���
    char              m_szCacheData[UNISTOR_MAX_KV_SIZE]; ///<�洢��data
    CWX_UINT32        m_uiCacheDataLen; ///<data�ĳ���
    bool              m_bCacheValue; ///<�Ƿ�洢store��ֵ��
    bool              m_bCacheMore; ///<store���Ƿ���ֵ
    bool              m_bCacheFirst; ///<�Ƿ���cache�ĵ�һ����
    bool              m_bCacheDel; ///<cache�Ƿ��Ѿ�ɾ��

    char			  m_szStoreKey[UNISTOR_MAX_KEY_SIZE]; ///<�洢��key
    CWX_UINT16        m_unStoreKeyLen; ///<key�ĳ���
    char              m_szStoreData[UNISTOR_MAX_KV_SIZE]; ///<�洢��data
    CWX_UINT32        m_uiStoreDataLen; ///<data�ĳ���
    bool              m_bStoreValue; ///<�Ƿ�洢store��ֵ��
    bool              m_bStoreMore; ///<store���Ƿ���ֵ
};

///�洢����ӿ���
class UnistorStoreBase{
public:
    ///���캯��
    UnistorStoreBase(){
        m_pMsgPipeFunc = NULL;
        m_pMsgPipeApp = NULL;
		m_binLogMgr = NULL; 
		m_config = NULL; 
		m_bValid = false; 
        m_bEnableExpire = false; 
        m_ttExpireClock = 0; 
        m_uiDefExpire = 0;
		strcpy(m_szErrMsg, "No init.");
		m_szDataBuf = new char[UNISTOR_DEF_KV_SIZE];
		m_uiDataBufLen = UNISTOR_DEF_KV_SIZE;
        m_cache = NULL;
        m_ullStoreSid = 0;
        m_uiUncommitBinlogNum = 0;
        m_fnKeyGroup = NULL;
        m_fnKeyAsciiLess = NULL;

    }
    ///��������
    virtual ~UnistorStoreBase(){
		if (m_szDataBuf) delete [] m_szDataBuf;
		m_szDataBuf = NULL;
        if (m_cache) delete m_cache;
		close();
    }
public:
    //���������ļ�.-1:failure, 0:success
    virtual int init(UNISTOR_MSG_CHANNEL_FN msgPipeFunc, ///<�洢�������ϲ����Ϣͨ������
        void* msgPipeApp, ///<UnistorApp����
        UnistorConfig const* config ///<�����ļ�
        );

	///����Ƿ����key��1�����ڣ�0�������ڣ�-1��ʧ��
    virtual int isExist(UnistorTss* tss, ///tss����
        CwxKeyValueItem const& key, ///<����key
        CwxKeyValueItem const* field, ///<����field����Ϊ�ձ�ʾ���key
        CwxKeyValueItem const* extra, ///<�洢�����extra data
        CWX_UINT32& uiVersion, ///<����key�İ汾��
        CWX_UINT32& uiFieldNum ///<����key��field������
        )=0;
	
    ///���key��1���ɹ���0�����ڣ�-1��ʧ�ܣ�
    virtual int addKey(UnistorTss* tss, ///<tss����
        CwxKeyValueItem const& key, ///<��ӵ�key
        CwxKeyValueItem const* field, ///<��ӵ�field����ָ���������signֵ�����Ƿ����field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<���key��field������
        CWX_UINT32    uiSign, ///<��ӵı�־
        CWX_UINT32& uiVersion, ///<������0���������޸ĺ��keyΪ�˰汾�����򷵻��°汾
        CWX_UINT32& uiFieldNum, ///����<key field������
        bool bCache=true, ///<�Ƿ�key�ŵ���cache
        CWX_UINT32 uiExpire=0 ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        )=0;
	
    ///set key��1���ɹ���-1������0�������ڣ���������һ��key��fieldʱ��
    virtual int setKey(UnistorTss* tss,///tss
        CwxKeyValueItem const& key, ///<set��key
        CwxKeyValueItem const* field, ///<����set field����ָ��Ҫset��field
        CwxKeyValueItem const* extra, ///<�洢�����extra ����
        CwxKeyValueItem const& data, ///<set������
        CWX_UINT32 uiSign, ///<���õı��
        CWX_UINT32& uiVersion, ///<���õ�version��������0��������Ϊָ���İ汾�����򷵻�ָ���İ汾
        CWX_UINT32& uiFieldNum, ///<key�ֶε�����
        bool bCache=true, ///<�Ƿ�����ݽ���cache
        CWX_UINT32 uiExpire=0 ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        )=0;
    
    ///update key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�-2���汾����
    virtual int updateKey(UnistorTss* tss, ///<tss����
        CwxKeyValueItem const& key, ///<update��key
        CwxKeyValueItem const* field,///<��update field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra ����
        CwxKeyValueItem const& data, ///<update������
        CWX_UINT32 uiSign, ///<update�ı��
        CWX_UINT32& uiVersion, ///<��ָ������key�İ汾�������ֵһ�£��������ʧ��
        CWX_UINT32& uiFieldNum, ///<����key field������
        CWX_UINT32 uiExpire=0 ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        )=0;
	
    ///inc key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�-2:�汾����-3�������߽�
    virtual int incKey(UnistorTss* tss, ///<�߳�tss����
        CwxKeyValueItem const& key,  ///<inc��key
        CwxKeyValueItem const* field, ///<��Ҫincһ��field����������ָ����Ӧ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CWX_INT32 num, ///<���ӻ���ٵ�����
        CWX_INT64  llMax, ///<�������Ӷ��Ҵ�ֵ��Ϊ0����inc���ֵ���ܳ�����ֵ
        CWX_INT64  llMin, ///<���Ǽ��ٶ����ֵ��Ϊ0����dec���ֵ���ܳ�����ֵ
        CWX_UINT32  uiSign, ///<inc�ı��
        CWX_INT64& llValue, ///<inc��dec�����ֵ
        CWX_UINT32& uiVersion, ///<��ָ������key�İ汾�ű�����ڴ�ֵ������ʧ�ܡ������°汾�š�
        CWX_UINT32  uiExpire=0 ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        )=0;
	
    ///del key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�-2:�汾����
    virtual int delKey(UnistorTss* tss, ///<�߳�tss����
        CwxKeyValueItem const& key, ///<Ҫɾ����key
        CwxKeyValueItem const* field, ///<��Ҫɾ��field����ָ��field������
        CwxKeyValueItem const* extra,///<�洢�����extra ����
        CWX_UINT32& uiVersion, ///<��ָ���汾�ţ����޸�ǰ�İ汾�ű������ֵ��ȣ�����ʧ�ܡ������°汾��
        CWX_UINT32& uiFieldNum  ///<key���ֶ�����
        )=0;

    ///sync ���key��1���ɹ���0�����ڣ�-1��ʧ�ܣ�
    virtual int syncAddKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<key������
        CwxKeyValueItem const* field, ///<�ֶε�����
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<add������
        CWX_UINT32 uiSign, ///<add��sign
        CWX_UINT32 uiVersion, ///<�����İ汾��
        bool bCache, ///<�Ƿ�cache����
        CWX_UINT32 uiExpire, ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        CWX_UINT64 ullSid, ///<�����־��sidֵ
        bool  bRestore=false ///<�Ƿ��Ǵ�binlog�ָ�������
        )=0;
    
    ///sync set key��1���ɹ���-1������0�������ڣ���������һ��key��fieldʱ��
    virtual int syncSetKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<set��key
        CwxKeyValueItem const* field, ///<����set field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<set������
        CWX_UINT32 uiSign,  ///<set��sign
        CWX_UINT32 uiVersion, ///<set��key �汾��
        bool bCache, ///<�Ƿ�cache����
        CWX_UINT32 uiExpire, ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        CWX_UINT64 ullSid, ///<set binlog��sidֵ
        bool  bRestore=false ///<�Ƿ��Ǵ�binlog�ָ�������
        )=0;
    
    ///sync update key��1���ɹ���0�������ڣ�-1��ʧ��
    virtual int syncUpdateKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<update��key
        CwxKeyValueItem const* field, ///<����update field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<update��������
        CWX_UINT32 uiSign, ///<update�ı��
        CWX_UINT32 uiVersion, ///<update���key�İ汾��
        CWX_UINT32 uiExpire, ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        CWX_UINT64 ullSid, ///<update���binlog��sid
        bool  bRestore=false ///<�Ƿ��binlog�лָ�������
        )=0;
    
    ///sync inc key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�
    virtual int syncIncKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key,  ///<inc��key
        CwxKeyValueItem const* field, ///<���Ƕ�field����inc����ָ��field������
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CWX_INT32 num,  ///<inc����ֵ������Ϊ��ֵ
        CWX_INT64  llMax, ///<����inc��ֵ������ָ��llMax����inc���ֵ���ܳ�����ֵ
        CWX_INT64  llMin, ///<����������Сֵ
        CWX_UINT32 uiSign, ///<inc�ı��
        CWX_INT64& llValue, ///<inc�����ֵ
        CWX_UINT32 uiVersion, ///<inc���key�İ汾��
        CWX_UINT32 uiExpire, ///<������key������ָ����uiExpire������key�ĳ�ʱʱ��
        CWX_UINT64 ullSid, ///<inc����binlog��sidֵ
        bool  bRestore=false ///<�Ƿ��binlog�ָ�������
        )=0;
    
    ///sync del key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�
    virtual int syncDelKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<Ҫɾ����key
        CwxKeyValueItem const* field, ///<����ɾ��field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CWX_UINT32 uiVersion, ///<key����delete��İ汾��
        CWX_UINT64 ullSid, ///<delete������Ӧ��binlog��sid
        bool  bRestore=false ///<�Ƿ��binlog�ָ�������
        )=0;

	///��ȡkey, 1���ɹ���0�������ڣ�-1��ʧ��;
    virtual int get(UnistorTss* tss, ///<�߳�tss����
        CwxKeyValueItem const& key, ///<Ҫ��ȡ��key
        CwxKeyValueItem const* field, ///<����Ϊ�գ����ȡָ����field�����field��\n�ָ�
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        char const*& szData, ///<�����ڣ��򷵻����ݡ��ڴ��д洢�������
        CWX_UINT32& uiLen,  ///<szData���ݵ��ֽ���
        bool& bKeyValue,  ///<���ص������Ƿ�Ϊkey/value�ṹ
        CWX_UINT32& uiVersion, ///<���Ե�ǰ�İ汾��
        CWX_UINT32& uiFieldNum, ///<key�ֶε�����
        bool bKeyInfo=false ///<�Ƿ��ȡkey��information
        )=0;
    
    ///��ȡ���key, 1���ɹ���-1��ʧ��;
    virtual int gets(UnistorTss* tss, ///<�̵߳�tss����
        list<pair<char const*, CWX_UINT16> > const& keys,  ///<Ҫ��ȡ��key���б�pair��firstΪkey�����֣�secondΪkey�ĳ���
        CwxKeyValueItem const* field, ///<��ָ�������޶���ȡ��field��Χ
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        char const*& szData, ///<��ȡ�����ݣ��ڴ��ɴ洢�������
        CWX_UINT32& uiLen, ///<�������ݵĳ���
        bool bKeyInfo ///<�Ƿ������ȡkey��infomation��Ϣ
        )=0;

    ///�����αꡣ-1���ڲ�����ʧ�ܣ�0���ɹ�
    virtual int createCursor(UnistorStoreCursor& cursor, ///<�α����
        CwxKeyValueItem const* field, ///<ָ���α�Ҫ���ص�field��
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        char* szErr2K ///<���������ش�����Ϣ
        ) = 0;

	///��ȡ��һ��key���ݡ�-1��ʧ�ܣ�0��������1����ȡһ��
    virtual int next(UnistorTss* tss, ///<�̵߳�tss
        UnistorStoreCursor& cursor,  ///<Next���α�
        char const*& szKey,  ///<���ص�key���ڴ��ɴ洢�������
        CWX_UINT16& unKeyLen,  ///<����key���ֽ���
        char const *& szData,  ///<����key��data���ڴ��ɴ洢�������
        CWX_UINT32& uiDataLen, ///<����data���ֽ���
        bool& bKeyValue,  ///<data�Ƿ�ΪkeyValue�ṹ
        CWX_UINT32& uiVersion,  ///<key�İ汾��
        bool bKeyInfo=false ///<�Ƿ񷵻�key��information��������data
        )=0;
	
    ///�ر��α�
	virtual void closeCursor(UnistorStoreCursor& cursor) = 0;
    
    ///��ʼ�������ݡ�-1���ڲ�����ʧ�ܣ�0���ɹ�
    virtual int exportBegin(UnistorStoreCursor& cursor, ///<export���α�
        char const* szStartKey, ///<export�Ŀ�ʼkey����������key
        char const* szExtra, ///<extra��Ϣ
        UnistorSubscribe const& scribe,  ///<�������ݵĶ��Ĺ���
        CWX_UINT64& ullSid, ///<��ǰ��sidֵ
        char* szErr2K  ///<�������򷵻ش�����Ϣ
        ) = 0;

    ///��ȡ���ݡ�-1��ʧ�ܣ�0��������1����ȡһ����2��skip����Ϊ0
    virtual int exportNext(UnistorTss* tss,  ///<�̵߳�tss����
        UnistorStoreCursor& cursor,  ///<export���α�
        char const*& szKey,    ///<����key��ֵ
        CWX_UINT16& unKeyLen,   ///<key���ֽ���
        char const*& szData,    ///<����data��ֵ
        CWX_UINT32& uiDataLen,   ///<data���ֽ���
        bool& bKeyValue,   ///<data�Ƿ�ΪKeyValue�ṹ
        CWX_UINT32& uiVersion, ///<key�İ汾��
        CWX_UINT32& uiExpire, ///<key��expireʱ��
        CWX_UINT16& unSkipNum,  ///<��ǰ������skip��binlog����
        char const*& szExtra,  ///<extra����
        CWX_UINT32&  uiExtraLen ///<extra�ĳ���
        )=0;
    
    ///������������
    virtual void exportEnd(UnistorStoreCursor& cursor)=0;
	
    ///commit���ݣ�0���ɹ���-1��ʧ��
	virtual int commit(char* szErr2K)=0;
	
    ///ͬ��master��binlog.0���ɹ���-1��ʧ��
    virtual int syncMasterBinlog(UnistorTss* tss, ///<�߳�tss
        CwxPackageReader* reader,  ///<��data���н�����ⲿCwxPackageReader����
        CWX_UINT64 ullSid, ///<binlog��sidֵ
        CWX_UINT32 ttTimestamp, ///<binlog��ʱ���
        CWX_UINT32 uiGroup, ///<binlog�����ķ���
        CWX_UINT32 uiType,  ///<binlog����Ϣ����
        CwxKeyValueItem const& data, ///<binlog������
        CWX_UINT32 uiVersion, ///<binlog��Ӧ��key�İ汾��
        bool  bRestore=false  ///<�Ƿ��Ǵӱ���binlog�ָ�������
        );
    
    ///event��������ʵ�ִ洢�������ϲ�Ľ�����0���ɹ���-1��ʧ��
    virtual int storeEvent(UnistorTss* tss, ///<�̵߳�tss
        CwxMsgBlock*& msg  ///<��Ϣ
        )=0;
	
    ///�洢�����checkpoint
	virtual void checkpoint()=0;
	
    ///��ȡengine������
	virtual char const* getName() const = 0;
	
    ///��ȡengine�İ汾
	virtual char const* getVersion() const = 0;
	
    ///�رմ洢����
	virtual int close();
    
    ///�Ƿ���ָ��������
    virtual bool isSubscribe(UnistorSubscribe const& subscribe, ///<���Ķ���
        CWX_UINT32 uiGroup, ///<binlog�����ķ���
        char const* szKey ///<binlog��key
        )
    {
        return subscribe.isSubscribe(uiGroup, szKey);
    }
    
    ///��鶩�ĸ�ʽ�Ƿ�Ϸ�
    virtual bool isValidSubscribe(UnistorSubscribe const& subscribe,///<���Ķ���
        string& strErrMsg ///<���Ϸ�ʱ�Ĵ�����Ϣ
        );

public:
    
    ///��ȡcache����
    inline UnistorCache*  getCache() {
        return  m_cache;
    }
    
    ///����cache
    int startCache(CWX_UINT32 uiWriteCacheMBtye, ///<write cache�Ĵ�С����Ϊ0��ʾû��write cache
        CWX_UINT32 uiReadCacheMByte, ///<��cache�Ĵ�С����Ϊ0��ʾû��read cache
        CWX_UINT32 uiReadMaxCacheKeyNum, ///<��cache�����cache��Ŀ
        UNISTOR_WRITE_CACHE_WRITE_BEGIN_FN fnBeginWrite, ///<д�Ŀ�ʼ����
        UNISTOR_WRITE_CACHE_WRITE_WRITE_FN   fnWrite, ///<д����
        UNISTOR_WRITE_CACHE_WRITE_END_FN     fnEndWrite, ///<д��������
        void*     context, ///<������context
        UNISTOR_KEY_CMP_EQUAL_FN  fnEqual, ///<key����ȱȽϺ���
        UNISTOR_KEY_CMP_LESS_FN   fnLess,  ///<key��less�ȽϺ���
        UNISTOR_KEY_HASH_FN       fnHash,   ///<key��hashֵ��ȡ����
        CWX_UINT16     unAlign=64 ///<cache�ռ�Ķ����ֽ���
        );

    ///��ȡ��һ��cache���ݡ�-1��ʧ�ܣ�0��������1����ȡһ��
    int nextCache(UnistorStoreCursor& cursor, ///<cursor����
        char* szErr2K ///<ʧ��ʱ�Ĵ�����Ϣ��һ�����ڴ治��
        );

	///��ȡָ����С��buf������NULL��ʾʧ��
	inline char* getBuf(CWX_UINT32 uiDataLen){
		if (uiDataLen > m_uiDataBufLen){
			delete [] m_szDataBuf;
			m_szDataBuf = new char[uiDataLen];
			if (m_szDataBuf)
				m_uiDataBufLen = uiDataLen;
			else
				m_uiDataBufLen = 0;
		}
		return m_szDataBuf;
	}
	
    ///��binlog�лָ����ݡ�0���ɹ���-1��ʧ��
	int restore(CWX_UINT64 ullSid/*��ʼ�ָ���sidֵ*/);

    ///get binlog
	inline CwxBinLogMgr* getBinLogMgr() {
        return m_binLogMgr;
    }

    ///flush binlog��0���ɹ���-1��ʧ��
	inline int flushBinlog(char* szErr2K=NULL){
		int ret = 0;
		CWX_INFO(("Begin flush binlog....................."));
		if (m_binLogMgr){
			ret = m_binLogMgr->commit(false, szErr2K);
		}
		CWX_INFO(("End flush binlog....................."));
		return ret;
	}

    ///���Add key binlog.0���ɹ���-1��ʧ��
    int appendTimeStampBinlog(CwxPackageWriter& writer, ///<writer����
        CWX_UINT32      ttNow, ///<��ǰ��ʱ��
        char* szErr2K=NULL ///<�������򷵻ش�����Ϣ
        );

    ///���Add key binlog.0���ɹ���-1��ʧ��
    int appendAddBinlog(CwxPackageWriter& writer, ///<write����
        CwxPackageWriter& writer1, ///<write����
        CWX_UINT32 uiGroup, ///<�����ķ���
        CwxKeyValueItem const& key, ///<binlog��key
        CwxKeyValueItem const* field, ///<binlog��field
        CwxKeyValueItem const* extra, ///<binlog��extra����
        CwxKeyValueItem const& data, ///<data����
        CWX_UINT32    uiExpire, ///<ʧЧʱ��
        CWX_UINT32    uiSign, ///<add key�ı��
        CWX_UINT32    uiVersion, ///<key add��İ汾��
        bool          bCache, ///<�Ƿ�cache����
        char*		  szErr2K=NULL ///<����ʱ�Ĵ�����Ϣ
        );

    ///���set key binlog.0���ɹ���-1��ʧ��
    int appendSetBinlog(CwxPackageWriter& writer, ///<write����
        CwxPackageWriter& writer1, ///<write����
        CWX_UINT32 uiGroup, ///<�����ķ���
        CwxKeyValueItem const& key, ///<binlog��key
        CwxKeyValueItem const* field, ///<binlog��field
        CwxKeyValueItem const* extra, ///<binlog��extra����
        CwxKeyValueItem const& data, ///<data����
        CWX_UINT32    uiExpire, ///<ʧЧʱ��
        CWX_UINT32    uiSign, ///<set key�ı��
        CWX_UINT32    uiVersion, ///<key set��İ汾��
        bool          bCache, ///<�Ƿ�cache����
        char*		  szErr2K=NULL ///<����ʱ�Ĵ�����Ϣ
        );

    ///���update key binlog.0���ɹ���-1��ʧ��
    int appendUpdateBinlog(CwxPackageWriter& writer, ///<write����
        CwxPackageWriter& writer1, ///<write����
        CWX_UINT32 uiGroup, ///<�����ķ���
        CwxKeyValueItem const& key, ///<binlog��key
        CwxKeyValueItem const* field, ///<binlog��field
        CwxKeyValueItem const* extra, ///<binlog��extra����
        CwxKeyValueItem const& data, ///<data����
        CWX_UINT32    uiExpire, ///<ʧЧʱ��
        CWX_UINT32    uiSign, ///<update key�ı��
        CWX_UINT32    uiVersion, ///<key update��İ汾��
        char*		  szErr2K=NULL ///<����ʱ�Ĵ�����Ϣ
        );

    ///���inc key binlog.0���ɹ���-1��ʧ��
    int appendIncBinlog(CwxPackageWriter& writer, ///<write����
        CwxPackageWriter& writer1, ///<write����
        CWX_UINT32 uiGroup, ///<�����ķ���
        CwxKeyValueItem const& key, ///<binlog��key
        CwxKeyValueItem const* field, ///<binlog��field
        CwxKeyValueItem const* extra, ///<binlog��extra����
        int	             num, ///<inc����ֵ
        CWX_INT64        llMax, ///<�����������ֵ
        CWX_INT64        llMin, ///<����������Сֵ
        CWX_UINT32        uiExpire, ///<ʧЧʱ��
        CWX_UINT32       uiSign, ///<Inc�ı��
        CWX_UINT32       uiVersion, ///<key inc��İ汾��
        char*			 szErr2K  ///<����ʱ�Ĵ�����Ϣ
        );

    ///���del key binlog.0���ɹ���-1��ʧ��
    int appendDelBinlog(CwxPackageWriter& writer, ///<write����
        CwxPackageWriter& writer1, ///<write����
        CWX_UINT32 uiGroup, ///<�����ķ���
        CwxKeyValueItem const& key, ///<binlog��key
        CwxKeyValueItem const* field, ///<binlog��field
        CwxKeyValueItem const* extra, ///<binlog��extra����
        CWX_UINT32       uiVersion, ///<keyɾ����İ汾��
        char*			 szErr2K ///<����ʱ�Ĵ�����Ϣ
        );

	///append binlog��0���ɹ���-1��ʧ��
	inline int appendBinlog(CWX_UINT64& ullSid, ///<binlog��sid
		CWX_UINT32 ttTimestamp, ///<binlog��ʱ���
		CWX_UINT32 uiGroup, ///<binlog�����ķ���
		char const* szData, ///<binlog��data
		CWX_UINT32 uiDataLen, ///<binlog data���ֽ���
		char* szErr2K=NULL  ///<����ʱ�Ĵ�����Ϣ
        )
    {
		int ret = m_binLogMgr->append(ullSid,
            ttTimestamp,
            uiGroup,
            szData,
            uiDataLen,
            szErr2K);
		if (0 != ret) return -1;
		return 0;
	}

    ///��ȡ��ǰ��sidֵ
	inline CWX_UINT64 getCurSid() const{
		return m_binLogMgr->getCurNextSid();
	}

    ///���õ�ǰ��sidֵ
    inline void setCurSid(CWX_UINT64 ullSid){
        m_binLogMgr->setNextSid(ullSid);
    }

    ///��ȡ��ǰexpireʱ���
    inline CWX_UINT32 getExpireClock() const{
        return m_ttExpireClock;
    }

    ///���õ�ǰ��expireʱ���
    inline void setExpireClock(CWX_UINT32 ttClock){
        m_ttExpireClock = ttClock;
    }

    ///��ȡ������Ϣ
	inline UnistorConfig const* getConfig() const{
        return m_config;
    }

	///�洢�Ƿ���Ч
	inline bool isValid() const {
        return m_bValid;
    }

	///�洢��Чʱ�Ĵ�����Ϣ
	char const* getErrMsg() const {
        return m_szErrMsg;
    }
    
    //��ȡkey�ķ���
    inline CWX_UINT32 getKeyGroup(char const* szKey, ///<key
        CWX_UINT16 unKeyLen ///<key�ĳ���
        )
    {
        return m_fnKeyGroup(szKey, unKeyLen);
    }
    
    ///key��ascii����less������0����ȣ�-1��С�ڣ�1������
    inline int isKeyAsciiLess(char const* key1, ///<��һ��key
        CWX_UINT16 unKey1Len, ///<��һ��key�ĳ���
        char const* key2, ///<�ڶ���key
        CWX_UINT16 unKey2Len ///<�ڶ���key�ĳ���
        )
    {
        return m_fnKeyAsciiLess(key1, unKey1Len, key2, unKey2Len);
    }

public:
    ///������[\n]�ָ��Ķ���ֶΡ����ؿո�ָ����ֶε�����
	static int parseMultiField(char const* szFields, ///<��\n�ָ�Ķ���ֶ��ַ��� 
        UnistorKeyField*& field ///<�������ֶΡ�
        );
	
    ///�ͷ�field�ֶ�
	static void freeField(UnistorKeyField*& key);
	
    ///��add key���¡���field���й鲢��-1��ʧ�ܣ�0�����ڣ�1���ɹ�
    static int mergeAddKeyField(CwxPackageWriter* writer1, ///<writer����
        CwxPackageReader* reader1, ///<reader ����
        CwxPackageReader* reader2, ///<reader ����
		char const* key, ///<key������
        CwxKeyValueItem const* field, ///<field������
		char const* szOldData, ///<key��ԭ����
		CWX_UINT32 uiOldDataLen, ///<keyԭ���ݵĳ���
		bool bOldKeyValue, ///<keyԭ�����Ƿ�Ϊkey/value
		char const* szNewData, ///<key��������
		CWX_UINT32 uiNewDataLen, ///<key�����ݵĳ���
		bool bNewKeyValue, ///<key�������Ƿ�Ϊkey/value
        CWX_UINT32& uiFieldNum, ///<merge����ֶ���
        char* szErr2K ///<mergeʧ�ܵĴ�����Ϣ
        );
	
    ///��set key���¡���field���й鲢��-1��ʧ�ܣ�1���ɹ�
    static int mergeSetKeyField(CwxPackageWriter* writer1, ///<writer����
        CwxPackageReader* reader1, ///<reader ����
        CwxPackageReader* reader2, ///<reader ����
        char const* key, ///<key������
        CwxKeyValueItem const* field, ///<field������
        char const* szOldData, ///<key��ԭ����
        CWX_UINT32 uiOldDataLen, ///<keyԭ���ݵĳ���
        bool bOldKeyValue, ///<keyԭ�����Ƿ�Ϊkey/value
        char const* szNewData, ///<key��������
        CWX_UINT32 uiNewDataLen, ///<key�����ݵĳ���
        bool bNewKeyValue, ///<key�������Ƿ�Ϊkey/value
        CWX_UINT32& uiFieldNum, ///<merge����ֶ���
        char* szErr2K ///<mergeʧ�ܵĴ�����Ϣ
        );
	
    ///��update key���¡���field���й鲢��-1��ʧ�ܣ�0�������ڣ�1���ɹ�
    static int mergeUpdateKeyField(CwxPackageWriter* writer1, ///<writer����
        CwxPackageReader* reader1, ///<reader ����
        CwxPackageReader* reader2, ///<reader ����
        char const* key, ///<key������
        CwxKeyValueItem const* field, ///<field������
        char const* szOldData, ///<key��ԭ����
        CWX_UINT32 uiOldDataLen, ///<keyԭ���ݵĳ���
        bool bOldKeyValue, ///<keyԭ�����Ƿ�Ϊkey/value
        char const* szNewData, ///<key��������
        CWX_UINT32 uiNewDataLen, ///<key�����ݵĳ���
        bool bNewKeyValue, ///<key�������Ƿ�Ϊkey/value
        CWX_UINT32& uiFieldNum, ///<merge����ֶ���
        bool bAppend, ///<update merge��ʱ�򣬶��ڲ����ڵ����ֶ��Ƿ�append���
        char* szErr2K ///<mergeʧ�ܵĴ�����Ϣ
        );

	///��int key�ĸ��¡�-2��key�����߽磻-1��ʧ�ܣ�0�������ڣ�1���ɹ�;
    static int mergeIncKeyField(CwxPackageWriter* writer1, ///<writer����
        CwxPackageReader* reader1, ///<reader ����
        char const* key, ///<key������
        CwxKeyValueItem const* field, ///<field������
        char const* szOldData, ///<key��ԭ����
        CWX_UINT32 uiOldDataLen, ///<keyԭ���ݵĳ���
        bool bOldKeyValue, ///<keyԭ�����Ƿ�Ϊkey/value
		int  num,  ///<���ӵ�ֵ
		CWX_INT64  llMax, ///<�����������ֵ
		CWX_INT64  llMin, ///<����������Сֵ
		CWX_INT64& llValue, ///<����������ֵ
		char* szBuf,  ///<merge�����ݵ�buf
		CWX_UINT32& uiBufLen, ///<����buf��С�����������ݵ��ֽ���
		bool& bKeyValue, ///<�������Ƿ�Ϊkey/vaue
        CWX_UINT32 uiSign, ///<merge�ı�־
        char* szErr2K ///<mergeʧ�ܵĴ�����Ϣ
        );
	
    ///��delete  key��field��-1��ʧ�ܣ�0�������ڣ�1���ɹ�
    static int mergeRemoveKeyField(CwxPackageWriter* writer1, ///<writer����
        CwxPackageReader* reader1, ///<reader ����
        char const* key, ///<key������
        CwxKeyValueItem const* field, ///<field������
        char const* szOldData, ///<key��ԭ����
        CWX_UINT32 uiOldDataLen, ///<keyԭ���ݵĳ���
        bool bOldKeyValue, ///<keyԭ�����Ƿ�Ϊkey/value
        bool bSync, ///<�Ƿ��Ǵ�binlog��ͬ������
        CWX_UINT32& uiFieldNum, ///<�����ֶε�����
        char* szErr2K ///<mergeʧ�ܵĴ�����Ϣ
        );
    
    ///��ȡָ����field, UNISTOR_ERR_SUCCESS���ɹ����������������
    static int pickField(CwxPackageReader& reader, ///<reader����
        CwxPackageWriter& write, ///<writer����
        UnistorKeyField const* field, ///<Ҫ��ȡ��field�б�
        char const* szData, ///<key��field ����
        CWX_UINT32 uiDataLen, ///<key�����ݵĳ���
        char* szErr2K ///<��ȡʧ�ܵĴ�����Ϣ
        );

    //unpack��data��field��-1��ʧ�ܣ�0������kv�ṹ��1���ɹ�
    inline int unpackFields(CwxPackageReader& reader, ///<reader����
        char const* szData, ///<fields��key/value����
        CWX_UINT32 uiDataLen, ///<fields��key/value���ݵĳ���
        CWX_UINT32& uiExpire, ///<���ݵ�ʧЧʱ��
        CWX_UINT32& uiVersion ///<���ݵİ汾��
        )
    {
        if (!isKvData(szData, uiDataLen)) return 0;
        getKvVersion(szData, uiDataLen, uiExpire, uiVersion);
        if (!reader.unpack(szData, uiDataLen-5, false, true)){
            return -1;
        }
        return 1;
    }

    ///��ȡ��ʱʱ��
    inline CWX_UINT32 getNewExpire(CWX_UINT32 uiExpire ///<��ʱʱ��
        )
    {
        if (uiExpire > 3600 * 24 * 365){
            return uiExpire;
        }else if (uiExpire){
            return uiExpire + m_ttExpireClock;
        }
        return m_ttExpireClock + m_config->getCommon().m_uiDefExpire;
    }
    
    //key��dataΪfield�ṹ��true���ǣ�false������
    inline bool isKvData(char const* szData, CWX_UINT32 uiDataLen){
        if (!szData || (uiDataLen<9)) return false;
        return szData[uiDataLen-1] == 0?false:true;
    }
    
    //��ȡdata��version
    inline void getKvVersion(char const* szData, ///<data����
        CWX_UINT32 uiDataLen, ///<data�ĳ���
        CWX_UINT32& uiExpire, ///<key��ʧЧʱ��
        CWX_UINT32& uiVersion ///<key�İ汾��
        )
    {
        CWX_ASSERT(uiDataLen >= 9);
        memcpy(&uiVersion, szData + (uiDataLen-5), 4);
        memcpy(&uiExpire, szData + (uiDataLen - 9), 4);
    }
    
    ///����data��extra����
    inline void setKvDataSign(char* szData, ///<key��data
        CWX_UINT32& uiDataLen, ///<���뵱ǰdata�ĳ��ȣ������³���
        CWX_UINT32 uiExpire, ///<ʧЧʱ��
        CWX_UINT32 uiVersion, ///<key�İ汾��
        bool bKeyValue ///<data��key/value���
        )
    {
        memcpy(szData + uiDataLen, &uiExpire, sizeof(uiExpire));
        uiDataLen += sizeof(uiExpire);
        memcpy(szData + uiDataLen, &uiVersion, sizeof(uiVersion));
        uiDataLen += sizeof(uiVersion);
        szData[uiDataLen] = bKeyValue?1:0;
        uiDataLen++;
    }
    
    ///��ȡdata��extra���ݳ���
    inline CWX_UINT32 getKvDataSignLen() const {
        return sizeof(CWX_UINT32) + sizeof(CWX_UINT32) + 1;
    }
    
    ///��ȡkey��group����
    static inline UNISTOR_KEY_GROUP_FN getKeyGroupFn(){
        return m_fnKeyGroup;
    }
    
    ///��ȡkey��ascii��less�Ƚ�
    static inline UNISTOR_KEY_ASCII_LESS getKeyAsciiLess(){
        return m_fnKeyAsciiLess;
    }
protected:
    UNISTOR_MSG_CHANNEL_FN     m_pMsgPipeFunc; ///<��Ϣͨ����function
    void*                   m_pMsgPipeApp; ///<app����
	CwxBinLogMgr*			m_binLogMgr; ///<binlog
	UnistorConfig const*    m_config; ///<ϵͳ����
	string				    m_strEngine; ///<engine������
	bool					m_bValid; ///<�����Ƿ���Ч
    bool                    m_bEnableExpire; ///<�Ƿ�֧�ֳ�ʱ
    volatile CWX_UINT32     m_ttExpireClock; ///<��ǰʧЧ��ʱ���
    CWX_UINT32              m_uiDefExpire; ///<ȱʡ�ĳ�ʱʱ��
	char					m_szErrMsg[2048]; ///<������Чʱ�Ĵ�����Ϣ
	char*					m_szDataBuf;  ///<��ʱbuf
	CWX_UINT32				m_uiDataBufLen; ///<��ʱbuf�Ĵ�С
    CWX_UINT64              m_ullStoreSid;  ///<�洢��sid
    volatile CWX_UINT32	    m_uiUncommitBinlogNum; ///<δ�ύ��binlog����
    UnistorCache*           m_cache;               ///<cache����
    CWX_UINT32              m_uiWriteCacheMSize;   ///<дcache��MBYTE
    CWX_UINT32              m_uiReadCacheMSize;     ///<��cache��MBYTE
    CWX_UINT32              m_uiReadCacheItemNum;   ///<��cache�����cache��key������
    static UNISTOR_KEY_GROUP_FN    m_fnKeyGroup;    ///<key��group�����ȡ����
    static UNISTOR_KEY_ASCII_LESS  m_fnKeyAsciiLess;///<key��ascii less�ȽϺ���
};

#endif
