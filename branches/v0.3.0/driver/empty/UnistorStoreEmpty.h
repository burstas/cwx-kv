#ifndef __UNISTOR_STORE_EMPTY_H__
#define __UNISTOR_STORE_EMPTY_H__


#include "UnistorMacro.h"
#include "UnistorConfig.h"
#include "UnistorTss.h"
#include "CwxFile.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "UnistorStoreBase.h"



//1��key�Ľṹ�ǣ������ַ���key��
//2�����е�key�����ַ�������
//3��ϵͳ����дcache����дcache���򳬹�ָ����������дcache flush��ȥ�����Ƕ�������
//4����������ʱ��binglog��ӵ�ǰ���ֵ����1ǧ��Ȼ��ظ��ڴ�
//5�����ڷ�ϵͳkey���洢������Ϊ[data][expire][version][sign]�Ľṹ��expireΪ32λ������versionΪ32λ��������sign=1��ʾdataΪkv�ṹ��������kv�ṹ

/********************д���뵥�߳�*************************/

///������غ���
extern "C" {
	UnistorStoreBase* unistor_create_engine();
}

///�����ļ���empty��������
class UnistorConfigEmpty{
public:
    ///���캯��
	UnistorConfigEmpty(){
	}
public:
};

///empty��cursor�������
class UnistorStoreEmptyCursor{
public:
    ///���캯��
	UnistorStoreEmptyCursor(){
		m_bFirst = true;
	}
    ///��������
	~UnistorStoreEmptyCursor(){
	}
public:
	bool				  m_bFirst; ///<�Ƿ��ǵ�һ����ȡ
};


///bdb�Ĵ洢����
class UnistorStoreEmpty : public UnistorStoreBase{
public:
    ///���캯��
    UnistorStoreEmpty(){
        m_exKey = NULL;
        m_unExKeyNum = 0;
        m_unExKeyPos = 0;
        UnistorStoreBase::m_fnKeyAsciiLess = UnistorStoreEmpty::keyAsciiCmpLess;
        UnistorStoreBase::m_fnKeyGroup = UnistorStoreEmpty::keyGroup;
    }
    ///��������
    ~UnistorStoreEmpty(){
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
        );
    
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
        CWX_UINT32 uiExpire=0 ///<����Ϊ0����ָ��key��expireʱ�䡣����Ҫ�洢����֧��
        );


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
        CWX_UINT32 uiExpire=0 ///<��ָ�������޸�key��expireʱ�䡣����Ҫ�洢����֧��
        );    

    ///update key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�-2���汾����
    virtual int updateKey(UnistorTss* tss, ///<tss����
        CwxKeyValueItem const& key, ///<update��key
        CwxKeyValueItem const* field,///<��update field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra ����
        CwxKeyValueItem const& data, ///<update������
        CWX_UINT32 uiSign, ///<update�ı��
        CWX_UINT32& uiVersion, ///<��ָ������key�İ汾�������ֵһ�£��������ʧ��
        CWX_UINT32& uiFieldNum, ///<����key field������
        CWX_UINT32 uiExpire=0 ///<��ָ�������޸�key��expireʱ��
        );

    
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
        );


    ///delete key��1���ɹ���0�������ڣ�-1��ʧ�ܣ�-2:�汾����
    virtual int delKey(UnistorTss* tss, ///<�߳�tss����
        CwxKeyValueItem const& key, ///<Ҫɾ����key
        CwxKeyValueItem const* field, ///<��Ҫɾ��field����ָ��field������
        CwxKeyValueItem const* extra,///<�洢�����extra ����
        CWX_UINT32& uiVersion, ///<��ָ���汾�ţ����޸�ǰ�İ汾�ű������ֵ��ȣ�����ʧ�ܡ������°汾��
        CWX_UINT32& uiFieldNum  ///<key���ֶ�����
        );


    ///sync ���key��1���ɹ���0�����ڣ�-1��ʧ�ܣ�
    virtual int syncAddKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<key������
        CwxKeyValueItem const* field, ///<�ֶε�����
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<add������
        CWX_UINT32 uiSign, ///<add��sign
        CWX_UINT32 uiVersion, ///<�����İ汾��
        bool bCache, ///<�Ƿ�cache����
        CWX_UINT32 uiExpire, ///<key��expireʱ��
        CWX_UINT64 ullSid, ///<�����־��sidֵ
        bool  bRestore=false ///<�Ƿ��Ǵ�binlog�ָ�������
        );

    ///sync set key��1���ɹ���-1������0�������ڣ���������һ��key��fieldʱ��
    virtual int syncSetKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<set��key
        CwxKeyValueItem const* field, ///<����set field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<set������
        CWX_UINT32 uiSign,  ///<set��sign
        CWX_UINT32 uiVersion, ///<set��key �汾��
        bool bCache, ///<�Ƿ�cache����
        CWX_UINT32 uiExpire, ///<expireʱ��
        CWX_UINT64 ullSid, ///<set binlog��sidֵ
        bool  bRestore=false ///<�Ƿ��Ǵ�binlog�ָ�������
        );

    ///sync update key��1���ɹ���0�������ڣ�-1��ʧ��
    virtual int syncUpdateKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<update��key
        CwxKeyValueItem const* field, ///<����update field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CwxKeyValueItem const& data, ///<update��������
        CWX_UINT32 uiSign, ///<update�ı��
        CWX_UINT32 uiVersion, ///<update���key�İ汾��
        CWX_UINT32 uiExpire, ///<update��expireʱ��
        CWX_UINT64 ullSid, ///<update���binlog��sid
        bool  bRestore=false ///<�Ƿ��binlog�лָ�������
        );

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
        CWX_UINT32 uiExpire, ///<update��expireʱ��
        CWX_UINT64 ullSid, ///<inc����binlog��sidֵ
        bool  bRestore=false ///<�Ƿ��binlog�ָ�������
        );

    ///sync delete key��1���ɹ���0�������ڣ�-1��ʧ��
    virtual int syncDelKey(UnistorTss* tss, ///<�̵߳�tss����
        CwxKeyValueItem const& key, ///<Ҫɾ����key
        CwxKeyValueItem const* field, ///<����ɾ��field����ָ��field
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        CWX_UINT32 uiVersion, ///<key����delete��İ汾��
        CWX_UINT64 ullSid, ///<delete������Ӧ��binlog��sid
        bool  bRestore=false ///<�Ƿ��binlog�ָ�������
        );

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
        );

    ///��ȡ���key, 1���ɹ���-1��ʧ��;
    virtual int gets(UnistorTss* tss, ///<�̵߳�tss����
        list<pair<char const*, CWX_UINT16> > const& keys,  ///<Ҫ��ȡ��key���б�pair��firstΪkey�����֣�secondΪkey�ĳ���
        CwxKeyValueItem const* field, ///<��ָ�������޶���ȡ��field��Χ
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        char const*& szData, ///<��ȡ�����ݣ��ڴ��ɴ洢�������
        CWX_UINT32& uiLen, ///<�������ݵĳ���
        bool bKeyInfo ///<�Ƿ������ȡkey��infomation��Ϣ
        );

	///�����αꡣ-1���ڲ�����ʧ�ܣ�0����֧�֣�1���ɹ�
    virtual int createCursor(UnistorStoreCursor& cursor, ///<�α����
        CwxKeyValueItem const* field, ///<ָ���α�Ҫ���ص�field��
        CwxKeyValueItem const* extra, ///<�洢�����extra����
        char* szErr2K ///<���������ش�����Ϣ
        );

	///��ȡ���ݡ�-1��ʧ�ܣ�0��������1����ȡһ��
    virtual int next(UnistorTss* tss, ///<�̵߳�tss
        UnistorStoreCursor& cursor,  ///<Next���α�
        char const*& szKey,  ///<���ص�key���ڴ��ɴ洢�������
        CWX_UINT16& unKeyLen,  ///<����key���ֽ���
        char const *& szData,  ///<����key��data���ڴ��ɴ洢�������
        CWX_UINT32& uiDataLen, ///<����data���ֽ���
        bool& bKeyValue,  ///<data�Ƿ�ΪkeyValue�ṹ
        CWX_UINT32& uiVersion,  ///<key�İ汾��
        bool bKeyInfo=false ///<�Ƿ񷵻�key��information��������data
        );
    	
    ///�ر��α�
	virtual void closeCursor(UnistorStoreCursor& cursor);
    
    ///��ʼ�������ݡ�-1���ڲ�����ʧ�ܣ�0���ɹ�
    virtual int exportBegin(UnistorStoreCursor& cursor, ///<export���α�
        char const* szStartKey, ///<export�Ŀ�ʼkey����������key
        char const* szExtra, ///<extra��Ϣ
        UnistorSubscribe const& scribe,  ///<�������ݵĶ��Ĺ���
        CWX_UINT64& ullSid, ///<��ǰ��sidֵ
        char* szErr2K  ///<�������򷵻ش�����Ϣ
        );
    
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
        );

    ///������������
    virtual void exportEnd(UnistorStoreCursor& cursor);

	///commit��0���ɹ���-1��ʧ��
	virtual int commit(char* szErr2K);
	
    ///�ر�bdb����
	virtual int close();

    ///event��������ʵ�ִ洢�������ϲ�Ľ�����0���ɹ���-1��ʧ��
    virtual int storeEvent(UnistorTss* tss, CwxMsgBlock*& msg);
	
    ///bdb����checkpoint
	virtual void checkpoint();
	
    ///��ȡengine������
	virtual char const* getName() const{
		return "empty";
	}
	
    ///��ȡengine�İ汾
	virtual char const* getVersion() const{
		return "1.0";
	}

private:
    ///dirty flush�߳�֪ͨ��ʼflush dirty���ݡ�����ֵ��0���ɹ���-1��ʧ��
    static int cacheWriteBegin(void* context, ///<��������Ϊbdb�������
        char* szErr2K ///<ʧ��ʱ�Ĵ�����Ϣ
        );
    ///dirty flush�߳�дdirty���ݣ�����ֵ��0���ɹ���-1��ʧ��
    static int cacheWrite(void* context, ///<��������Ϊbdb�������
        char const* szKey, ///<д���key
        CWX_UINT16 unKeyLen, ///<key�ĳ���
        char const* szData, ///<д���data
        CWX_UINT32 uiDataLen, ///<data�ĳ���
        bool bDel, ///<�Ƿ�key��ɾ��
        CWX_UINT32 ttOldExpire, ///<key�ڴ洢�е�expireʱ��ֵ
        char* szStoreKeyBuf, ///<key��buf
        CWX_UINT16 unKeyBufLen, ///<key buf�Ĵ�С
        char* szErr2K ///<ʧ��ʱ�Ĵ�����Ϣ
        );
    
    ///dirty flush�߳����dirty���ݵ�д�롣����ֵ����ֵ��0���ɹ���-1��ʧ��
    static int cacheWriteEnd(void* context, ///<��������Ϊbdb�������
        CWX_UINT64 ullSid, ///<д�����ݵ�sidֵ
        char* szErr2K ///<ʧ��ʱ�Ĵ�����Ϣ
        );
    
    ///key����ȱȽϺ���
    static bool keyCmpEqual(char const* key1,
        CWX_UINT16 unKey1Len,
        char const* key2,
        CWX_UINT16 unKey2Len)
    {
        return (unKey1Len == unKey2Len) && (memcmp(key1, key2, unKey1Len)==0);
    }
    
    ///key��С�ڱȽϺ���
    static int keyCmpLess(char const* key1,
        CWX_UINT16 unKey1Len,
        char const* key2,
        CWX_UINT16 unKey2Len)
    {
        int ret = memcmp(key1, key2, unKey1Len<unKey2Len?unKey1Len:unKey2Len);
        if (0 != ret) return ret;
        return unKey1Len==unKey2Len?0:(unKey1Len<unKey2Len?-1:1);
    }
    
    ///key��hash����
    static size_t keyCacheHash(char const* key, CWX_UINT16 unKeyLen){
        size_t h = 0;
        for (CWX_UINT16 i=0; i<unKeyLen; i++){
            h = 5*h + (unsigned char)key[i];
        }
        return h;
    }
    
    ///key��hash����
    static CWX_UINT32 keyGroup(char const* key, CWX_UINT16 unKeyLen){
        CWX_UINT32 uiGroup = 0;
        CwxMd5 md5;
        unsigned char szMd5[16];
        md5.update((unsigned char const*)key, unKeyLen);
        md5.final(szMd5);
        memcpy(&uiGroup, szMd5, 4);
        return uiGroup;
    }
    
    ///key��С�ڱȽϺ���
    static int keyAsciiCmpLess(char const* key1,
        CWX_UINT16 unKey1Len,
        char const* key2,
        CWX_UINT16 unKey2Len)
    {
        int ret = memcmp(key1, key2, unKey1Len<unKey2Len?unKey1Len:unKey2Len);
        if (0 != ret) return ret;
        return unKey1Len==unKey2Len?0:(unKey1Len<unKey2Len?-1:1);
    }
    
private:
    ///��ȡ���ݡ�-1��ʧ�ܣ�0��������1����ȡһ��
    int _nextEmpty(UnistorStoreCursor& cursor, char* szErr2K);

    ///commit��0���ɹ���-1��ʧ��
	int _commit(char* szErr2K);
	
    //����ϵͳ��Ϣ������ֵ��0:�ɹ���-1��ʧ��
	int _updateSysInfo(CWX_UINT64 ullSid, char* szErr2K);
	
    //����ϵͳ��Ϣ������ֵ��0:�ɹ���-1���ɹ�
	int _loadSysInfo(char* szErr2K);
	
    //����bdb��������Ϣ������ֵ0:�ɹ���-1��ʧ��
	int parseConf();

    //set key��0:�ɹ���-1��ʧ��
    int _setKey(char const* szKey, ///<key
        CWX_UINT16 unKeyLen,///<key�ĳ���
        char const* szData, ///<key��data
        CWX_UINT32 uiLen, ///<data�ĳ���
        CWX_UINT32 ttOldExpire, ///<��ǰ��expireʱ��ֵ
        bool bCache=true, ///<�Ƿ����cache�������������ɾ��cache
        char* szErr2K=NULL ///<����ʱ���ش�������
        );
    
    //��ȡkey��data������ֵ��0:�����ڣ�1����ȡ��-1��ʧ��
    int _getKey(char const* szKey, ///<key
        CWX_UINT16 unKeyLen, ///<key�ĳ���
        char* szData, ///<key��data
        CWX_UINT32& uiLen,///<����data��buf size������data�ĳ���
        char* szStoreKeyBuf, ///<key��buf
        CWX_UINT16 unKeyBufLen, ///<key��buf��С
        bool bCache=true, ///<�Ƿ�ʹ��cache
        char* szErr2K=NULL ///<����ʱ���ش�������
        );
    
    //ɾ��key��ͬʱ��cache��ɾ��������ֵ��0:�ɹ���-1��ʧ��
    int _delKey(char const* szKey, ///<key������
        CWX_UINT16 unKeyLen, ///<key�ĳ���
        CWX_UINT32 ttOldExpire, ///<key�ĵ�ǰexpireʱ��
        char* szErr2K=NULL ///<����ʱ���ش�������
        );

	//����empty��key��data������ֵ��0:�ɹ���-1��ʧ��
	int _setEmptyKey(char const* szKey, ///<key
        CWX_UINT16 unKeyLen, ///<key�ĳ���
        CWX_UINT16 unKeyBufLen, ///<key��buf�ռ��С
        char const* szData, ///<key��data
        CWX_UINT32 uiDataLen, ///<data�Ĵ�С
        char* szErr2K=NULL ///<����ʱ���ش�������
        );

    //��empty�л�ȡkey������ֵ��0:�����ڣ�1����ȡ��-1��ʧ��
	int _getEmptyKey(char const* szKey, ///<get��key
        CWX_UINT16 unKeyLen, ///<key�ĳ���
        char* szData,  ///<key��data���ռ���߱�֤
        CWX_UINT32& uiLen, ///<����szData�Ŀռ��С������data�Ĵ�С
        char* szStoreKeyBuf, ///<�洢key�Ŀռ�
        CWX_UINT16 unKeyBufLen, ///<�ռ��С
        char* szErr2K=NULL  ///<����ʱ���ش�������
        );

    //��empty��ɾ�����ݡ�����ֵ��0:�ɹ���-1��ʧ��
	int _delEmptyKey(char const* szKey, ///<ɾ����key
        CWX_UINT16 unKeyLen, ///<key�ĳ���
        CWX_UINT16 unKeyBufLen, ///<szKey�Ŀռ��С
        char* szErr2K=NULL  ///<����ʱ���ش�������
        );

    //����commit�¼���0���ɹ���-1��ʧ��
    int _dealCommitEvent(UnistorTss* tss, ///<�߳�tss
        CwxMsgBlock*& msg ///<��Ϣ
        );

    //���س�ʱ�����ݡ�0��û�������ݣ�1����ȡ�����ݣ�-1��ʧ��
    int _loadExpireData(UnistorTss* tss, ///<�߳�tss
        bool bJustContinue ///<������������keyʱ���Ƿ����¼���expire key
        );

    //���ͳ�ʱ���ݡ�0���ɹ���-1��ʧ��
    int _sendExpireData(UnistorTss* tss);

    //����expire�¼���0���ɹ���-1��ʧ��
    int _dealExpireEvent(UnistorTss* tss, ///<�߳�tss
        CwxMsgBlock*& msg ///<�ظ�����Ϣ
        );

    //����expire�¼��Ļظ���0���ɹ���-1��ʧ��
    int _dealExpireReplyEvent(UnistorTss* tss,  ///<�߳�tss
        CwxMsgBlock*& msg ///<�ظ�����Ϣ
        );

    //����modģʽ�Ķ��ġ�-1��ʧ�ܣ�0��������1����ȡһ����2��skip����Ϊ0
    int _exportNext(UnistorTss* tss,///<�߳�tss
        UnistorStoreCursor& cursor, ///<cursor����
        char const*& szKey, ///<��һ����key
        CWX_UINT16& unKeyLen, ///<key�ĳ���
        char const*& szData, ///<��һ��key��data
        CWX_UINT32& uiDataLen, ///<data�ĳ���
        bool& bKeyValue, ///<�Ƿ�dataΪkey/value
        CWX_UINT32& uiVersion, ///<key�İ汾��
        CWX_UINT32& uiExpire, ///<key�ĳ�ʱʱ��
        CWX_UINT16& unSkipNum ///<��ǰʣ���skip��
        );

    //export keyģʽ�Ķ��ġ�-1��ʧ�ܣ�0��������1����ȡһ����2��skip����Ϊ0
    int _exportKeyNext(UnistorTss* tss,///<�߳�tss
        UnistorStoreCursor& cursor, ///<cursor����
        char const*& szKey, ///<��һ����key
        CWX_UINT16& unKeyLen, ///<key�ĳ���
        char const*& szData, ///<��һ��key��data
        CWX_UINT32& uiDataLen, ///<data�ĳ���
        bool& bKeyValue, ///<�Ƿ�dataΪkey/value
        CWX_UINT32& uiVersion, ///<key�İ汾��
        CWX_UINT32& uiExpire, ///<key�ĳ�ʱʱ��
        CWX_UINT16& unSkipNum ///<��ǰʣ���skip��
        );

    //��ȡ��������һ��key�ķ�Χ��true���ɹ���false�����
    bool _exportKeyInit(string const& strKeyBegin, ///<��ǰexport���key
        string& strBegin, ///<��һ��key��Χ�Ŀ�ʼλ��
        string& strEnd, ///<��һ��key��Χ�Ľ���λ��
        UnistorSubscribeKey const& keys ///<key���Ĺ���
        );

private:
	UnistorConfigEmpty			m_engineConf; ///<bdb�������ļ�
    ///check expire����Ϣ
    pair<CWX_UINT32, UnistorStoreExpireKey*>*      m_exKey; ///<��ʱ��key cache
    CWX_UINT16                  m_unExKeyNum;     ///<ȡ��key������
    CWX_UINT16                  m_unExKeyPos;     ///<��һ�����͵�key��λ��
    list<CwxMsgBlock*>          m_exFreeMsg;      ///<���е�message
    char			            m_exStoreKey[sizeof(UnistorStoreExpireKey) + UNISTOR_MAX_KEY_SIZE]; ///<�洢��key
};

#endif
