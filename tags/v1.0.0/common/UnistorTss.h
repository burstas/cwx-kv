#ifndef __UNISTOR_TSS_H__
#define __UNISTOR_TSS_H__


#include "UnistorMacro.h"
#include "CwxLogger.h"
#include "CwxTss.h"
#include "CwxPackageReader.h"
#include "CwxPackageWriter.h"
#include "UnistorDef.h"



///����EVENT����
#define EVENT_ZK_CONNECTED      (CwxEventInfo::SYS_EVENT_NUM + 1) ///<��������zk������
#define EVENT_ZK_EXPIRED        (CwxEventInfo::SYS_EVENT_NUM + 2) ///<zk������ʧЧ
#define EVENT_ZK_FAIL_AUTH      (CwxEventInfo::SYS_EVENT_NUM + 3) ///<zk����֤ʧ��
#define EVENT_ZK_SUCCESS_AUTH   (CwxEventInfo::SYS_EVENT_NUM + 4) ///<zk����֤�ɹ� 
#define EVENT_ZK_CONF_CHANGE    (CwxEventInfo::SYS_EVENT_NUM + 5) ///<ZK��conf�ڵ����ñ仯
#define EVENT_ZK_LOCK_CHANGE    (CwxEventInfo::SYS_EVENT_NUM + 6) ///<ZK�����仯
#define EVENT_ZK_LOST_WATCH     (CwxEventInfo::SYS_EVENT_NUM + 7) ///<ʧȥwatch
#define EVENT_ZK_ERROR          (CwxEventInfo::SYS_EVENT_NUM + 8) ///<zk����
#define EVENT_ZK_SET_SID        (CwxEventInfo::SYS_EVENT_NUM + 9) ///<����zk��sid
#define EVENT_SEND_MSG          (CwxEventInfo::SYS_EVENT_NUM + 10) ///<������Ϣ
#define EVENT_STORE_MSG_START   (CwxEventInfo::SYS_EVENT_NUM + 100)  ///<�洢����Ϣ��ʼֵ
#define EVENT_STORE_COMMIT       EVENT_STORE_MSG_START ///<�洢����ִ����һ��commit
#define EVENT_STORE_DEL_EXPIRE   (EVENT_STORE_MSG_START + 2)  ///<ɾ��ָ���ĳ�ʱkey
#define EVENT_STORE_DEL_EXPIRE_REPLY (EVENT_STORE_MSG_START + 3) ///<ɾ��ָ���ĳ�ʱkey�Ļظ�

///tss���û��߳����ݶ���
class UnistorTssUserObj{
public:
    ///���캯��
    UnistorTssUserObj(){}
    ///��������
    virtual ~UnistorTssUserObj(){}
};

///Ϊ�洢engineԤ�������ö�����Ϣ
class UnistorTssEngineObj{
public:
    ///���캯��
    UnistorTssEngineObj(){}
    ///��������
    virtual ~UnistorTssEngineObj(){}

};

//unistor��recv�̳߳ص�tss
class UnistorTss:public CwxTss{
public:
    ///���캯��
    UnistorTss():CwxTss(){
        m_pZkConf = NULL;
        m_pZkLock = NULL;
        m_pReader = NULL;
        m_pItemReader = NULL;
        m_pEngineReader = NULL;
        m_pEngineItemReader = NULL;
        m_pWriter = NULL;
        m_pItemWriter = NULL;
        m_pEngineWriter = NULL;
        m_pEngineItemWriter = NULL;
        m_engineConf = NULL;
        m_uiThreadIndex = 0;
        m_szDataBuf = NULL;
        m_uiDataBufLen = 0;
        m_szDataBuf1 = NULL;
        m_uiDataBufLen1 = 0;
        m_szDataBuf2 = NULL;
        m_uiDataBufLen2 = 0;
        m_szDataBuf3 = NULL;
        m_uiDataBufLen3 = 0;
        m_userObj = NULL;
        m_uiBinLogVersion = 0;
        m_uiBinlogType = 0;
        m_pBinlogData = NULL;
    }
    ///��������
    ~UnistorTss();
public:
    ///tss�ĳ�ʼ����0���ɹ���-1��ʧ��
    int init(UnistorTssUserObj* pUserObj=NULL);
    ///��ȡ�û�������
    UnistorTssUserObj* getUserObj() { return m_userObj;}
    ///��ȡpackage��buf������NULL��ʾʧ��
    inline char* getBuf(CWX_UINT32 uiSize){
        if (m_uiDataBufLen < uiSize){
            delete [] m_szDataBuf;
            m_szDataBuf = new char[uiSize];
            m_uiDataBufLen = uiSize;
        }
        return m_szDataBuf;
    }
    ///��ȡpackage��buf������NULL��ʾʧ��
    inline char* getBuf1(CWX_UINT32 uiSize){
        if (m_uiDataBufLen1 < uiSize){
            delete [] m_szDataBuf1;
            m_szDataBuf1 = new char[uiSize];
            m_uiDataBufLen1 = uiSize;
        }
        return m_szDataBuf1;
    }
    ///��ȡpackage��buf������NULL��ʾʧ��
    inline char* getBuf2(CWX_UINT32 uiSize){
        if (m_uiDataBufLen2 < uiSize){
            delete [] m_szDataBuf2;
            m_szDataBuf2 = new char[uiSize];
            m_uiDataBufLen2 = uiSize;
        }
        return m_szDataBuf2;
    }
    ///��ȡpackage��buf������NULL��ʾʧ��
    inline char* getBuf3(CWX_UINT32 uiSize){
        if (m_uiDataBufLen3 < uiSize){
            delete [] m_szDataBuf3;
            m_szDataBuf3 = new char[uiSize];
            m_uiDataBufLen3 = uiSize;
        }
        return m_szDataBuf3;
    }
    ///�Ƿ���master idc
    bool isMasterIdc(){
        if (m_pZkConf && m_pZkConf->m_bMasterIdc) return true;
        return false;
    }
    ///��ȡmaster idc������
    char const* getMasterIdc() const{
        if (m_pZkConf && m_pZkConf->m_strMasterIdc.length()) return m_pZkConf->m_strMasterIdc.c_str();
        return "";
    }
    ///�Լ��Ƿ���master
    bool isMaster(){
        if (m_pZkLock && m_pZkLock->m_bMaster) return true;
        return false;
    }
    ///�Ƿ����master
    bool isExistMaster(){
        if (m_pZkLock && m_pZkLock->m_strMaster.length()) return true;
        return false;
    }
    ///��ȡmaster��������
    char const* getMasterHost() const{
        if (m_pZkLock && m_pZkLock->m_strMaster.length()) return m_pZkLock->m_strMaster.c_str();
        return "";
    }
    ///��ȡidc�ڵ���һ��sync��������
    char const* getSyncHost() const{
        if (m_pZkLock){
            if (m_pZkLock->m_strPrev.length()) return m_pZkLock->m_strPrev.c_str();
            if (m_pZkLock->m_strMaster.length()) return m_pZkLock->m_strMaster.c_str();
        }
        return "";
    }

public:
    CWX_UINT32              m_uiBinLogVersion; ///<binlog�����ݰ汾������binlog�ķַ�
    CWX_UINT32              m_uiBinlogType;   ///<binlog����Ϣ�����ͣ�����binlog�ķַ�
    CwxKeyValueItem const*  m_pBinlogData; ///<binlog��data������binglog�ķַ�

    UnistorZkConf*          m_pZkConf;  ///<zk�����ö���
    UnistorZkLock*          m_pZkLock;  ///<zk������Ϣ
    CwxPackageReader*       m_pReader; ///<���ݰ��Ľ������
    CwxPackageReader*       m_pItemReader; ///<���ݰ��Ľ������
    CwxPackageReader*       m_pEngineReader; ///<engineʹ�õ�reader���ⲿ����ʹ��
    CwxPackageReader*       m_pEngineItemReader; ///<engineʹ�õ�reader���洢���治��ʹ��
    CwxPackageWriter*       m_pWriter; ///<���ݰ���pack����
    CwxPackageWriter*       m_pItemWriter; ///<һ����Ϣ�����ݰ���pack����
    CwxPackageWriter*       m_pEngineWriter; ///<engineʹ�õ�writer���ⲿ����ʹ��
    CwxPackageWriter*       m_pEngineItemWriter; ///<engineʹ�õ�writer���ⲿ����ʹ��
    char			        m_szStoreKey[UNISTOR_MAX_KEY_SIZE]; ///<�洢��key
    CWX_UINT32              m_uiThreadIndex; ///<�̵߳�������
    UnistorTssEngineObj*    m_engineConf;       ///<engine��conf��Ϣ���������ʹ��
private:
    UnistorTssUserObj*    m_userObj;  ///�û�������
    char*                  m_szDataBuf; ///<����buf
    CWX_UINT32             m_uiDataBufLen; ///<����buf�Ŀռ��С
    char*                  m_szDataBuf1; ///<����buf1
    CWX_UINT32             m_uiDataBufLen1; ///<����buf1�Ŀռ��С
    char*                  m_szDataBuf2; ///<����buf2
    CWX_UINT32             m_uiDataBufLen2; ///<����buf2�Ŀռ��С
    char*                  m_szDataBuf3; ///<����buf3
    CWX_UINT32             m_uiDataBufLen3; ///<����buf3�Ŀռ��С
};









#endif
