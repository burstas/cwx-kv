#ifndef __UNISTOR_POCO_H__
#define __UNISTOR_POCO_H__

#include "UnistorMacro.h"
#include "CwxMsgBlock.h"
#include "CwxPackageReader.h"
#include "CwxPackageWriter.h"
#include "CwxCrc32.h"
#include "CwxMd5.h"


//unistor��Э�鶨�����
class UnistorPoco
{
public:
    enum ///<��Ϣ���Ͷ���
    {
        ///RECV�������͵���Ϣ���Ͷ���
        MSG_TYPE_TIMESTAMP = 0, ///<ʱ����Ϣ����
        MSG_TYPE_RECV_ADD = 1, ///<ADD key/value
        MSG_TYPE_RECV_ADD_REPLY = 2, ///<set key/value�Ļظ�
        MSG_TYPE_RECV_SET = 3, ///<SET key
        MSG_TYPE_RECV_SET_REPLY = 4, ///<SET key�Ļظ�
		MSG_TYPE_RECV_UPDATE = 5, ///update key
		MSG_TYPE_RECV_UPDATE_REPLY = 6, ///<update key�Ļظ�
		MSG_TYPE_RECV_INC = 7, ///<inc key
		MSG_TYPE_RECV_INC_REPLY = 8, ///<inc key�Ļظ�
		MSG_TYPE_RECV_DEL = 9, ///<del key
		MSG_TYPE_RECV_DEL_REPLY = 10, ///<del key�Ļظ�
        MSG_TYPE_RECV_EXIST = 11, ///<Key�Ƿ����
        MSG_TYPE_RECV_EXIST_REPLY = 12, ///<Key�Ƿ���ڵĻظ�
		MSG_TYPE_RECV_GET = 13, ///<get key
		MSG_TYPE_RECV_GET_REPLY = 14, ///<get key�Ļظ�
		MSG_TYPE_RECV_GETS = 15, ///<get ���key
		MSG_TYPE_RECV_GETS_REPLY = 16, ///<get ���key�Ļظ�
		MSG_TYPE_RECV_LIST = 17, ///<��ȡ�б�
		MSG_TYPE_RECV_LIST_REPLY = 18, ///<�б�ظ�
        MSG_TYPE_RECV_IMPORT = 19, ///<��ȡ�б�
        MSG_TYPE_RECV_IMPORT_REPLY = 20, ///<�б�ظ�
        MSG_TYPE_RECV_AUTH = 21, ///<��֤
        MSG_TYPE_RECV_AUTH_REPLY = 22, ///<��֤�ظ�
        ///���ݵ�����������
        MSG_TYPE_EXPORT_REPORT = 51, ///<���ݵ���export
        MSG_TYPE_EXPORT_REPORT_REPLY = 52, ///<���ݵ�����reply
        MSG_TYPE_EXPORT_DATA = 53, ///<���ݵ���������
        MSG_TYPE_EXPORT_DATA_REPLY = 54, ///<���ݵ���������reply
        MSG_TYPE_EXPORT_END = 55, ///<���ݵ������
        ///�ַ�����Ϣ���Ͷ���
        MSG_TYPE_SYNC_REPORT = 101, ///<ͬ��SID�㱨����Ϣ����
        MSG_TYPE_SYNC_REPORT_REPLY = 102, ///<report����
        MSG_TYPE_SYNC_CONN = 103, ///<����ͨ��
        MSG_TYPE_SYNC_CONN_REPLY = 104, ///<����ͨ���ظ�
        MSG_TYPE_SYNC_DATA = 105,  ///<��������
        MSG_TYPE_SYNC_DATA_REPLY = 106, ///<���ݵĻظ�
        MSG_TYPE_SYNC_DATA_CHUNK = 107,  ///<chunkģʽ��������
        MSG_TYPE_SYNC_DATA_CHUNK_REPLY = 108, ///<chunkģʽ�������ݵĻظ�
        MSG_TYPE_SYNC_ERR = 109 ///<sync�Ĵ�����Ϣ
    };
    enum
    {
        MAX_CONTINUE_SEEK_NUM = 8192
    };
public:
    ///pack Add key�����ݡ� ����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvImport(CwxPackageWriter* writer, ///<����pack��writer������ͨ��writer����
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiExpire=0,  ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        bool       bCache=true, ///<�Ƿ�cache����Ϊtrue�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack Add key����Ϣ���� ����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvImport(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiExpire=0,  ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        bool       bCache=true, ///<�Ƿ�cache����Ϊtrue�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///����Add key�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvImport(CwxPackageReader* reader, ///<reader
        CwxKeyValueItem const*& key,   ///<����key�ֶ�
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        CwxKeyValueItem const*& data,  ///<����data�ֶ�
        CWX_UINT32& uiExpire,  ///<����expire����Ϊ0��ʾû��ָ��
        CWX_UINT32& uiVersion, ///<���ذ汾
        bool&       bCache,    ///<����cache
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
        char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
        );



    ///pack Add key�����ݡ� ����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvAdd(CwxPackageWriter* writer, ///<����pack��writer������ͨ��writer����
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiExpire=0,  ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiSign=0,    ///<��ǣ���Ϊ0�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        bool       bCache=true, ///<�Ƿ�cache����Ϊtrue�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
        );

	///pack Add key����Ϣ���� ����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
	static int packRecvAdd(CwxPackageWriter* writer, ///<����pack��writer
		CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
		CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiExpire=0,  ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiSign=0,    ///<��ǣ���Ϊ0�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        bool       bCache=true, ///<�Ƿ�cache����Ϊtrue�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
		);

	///����Add key�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
	static int parseRecvAdd(CwxPackageReader* reader, ///<reader
        CwxKeyValueItem const*& key,   ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
		CwxKeyValueItem const*& data,  ///<����data�ֶ�
		CWX_UINT32& uiExpire,  ///<����expire����Ϊ0��ʾû��ָ��
        CWX_UINT32& uiSign,    ///<����sign
        CWX_UINT32& uiVersion, ///<���ذ汾
        bool&       bCache,    ///<����cache
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
		char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack set�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvSet(CwxPackageWriter* writer,///<����pack��writer������ͨ��writer����
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiSign=0, ///<��ǣ���Ϊ0�����
        CWX_UINT32 uiExpire=0, ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiVersion=0,///<�汾����Ϊ0�����
        bool   bCache=true, ///<�Ƿ�cache����Ϊtrue�����
        char const* user=NULL, ///<�û�����ΪNULL�����
        char const* passwd=NULL,///<�û������ΪNULL�����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack set����Ϣ�塣����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvSet(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiSign=0, ///<��ǣ���Ϊ0�����
        CWX_UINT32 uiExpire=0, ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiVersion=0,///<�汾����Ϊ0�����
        bool   bCache=true, ///<�Ƿ�cache����Ϊtrue�����
        char const* user=NULL, ///<�û�����ΪNULL�����
        char const* passwd=NULL,///<�û������ΪNULL�����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse set�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvSet(CwxPackageReader* reader,  ///<reader
        CwxKeyValueItem const*& key, ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        CwxKeyValueItem const*& data, ///<����data�ֶ�
        CWX_UINT32& uiSign, ///<����sign
		CWX_UINT32& uiExpire, ///<����expire
        CWX_UINT32& uiVersion, ///<���ذ汾
        bool&   bCache,  ///<����cache
        char const*& user, ///<�����û���NULL��ʾ������
        char const*& passwd, ///<���ؿ��NULL��ʾ������
        char* szErr2K=NULL  ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack update�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvUpdate(CwxPackageWriter* writer, ///<����pack��writer
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiSign=0, ///<��ǣ���Ϊ0�����
        CWX_UINT32 uiExpire=0, ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        char const* user=NULL, ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

	///pack update����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
	static int packRecvUpdate(CwxPackageWriter* writer, ///<����pack��writer
		CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
		CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CwxKeyValueItem const& data, ///<data
        CWX_UINT32 uiSign=0, ///<��ǣ���Ϊ0�����
        CWX_UINT32 uiExpire=0, ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        char const* user=NULL, ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
		);

	///parse update�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
	static int parseRecvUpdate(CwxPackageReader* reader, ///<reader
        CwxKeyValueItem const*& key, ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        CwxKeyValueItem const*& data, ///<����data�ֶ�
        CWX_UINT32& uiSign, ///<����sign
        CWX_UINT32& uiExpire, ///<����expire����Ϊ0��ʾû��ָ��
        CWX_UINT32& uiVersion, ///<���ذ汾
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
        char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
		);

    ///pack inc�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvInc(CwxPackageWriter* writer, ///<����pack��writer
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CWX_INT64   num, ///<inc�����֣������ɸ�
        CWX_INT64   max=0, ///<��incΪ��ֵ����ͨ��max�޶����ֵ
        CWX_INT64   min=0, ///<��incΪ��ֵ����ͨ��min�޶���Сֵ
        CWX_UINT32  uiExpire=0, ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32  uiSign=0, ///<��ǣ���Ϊ0�����
        CWX_UINT32  uiVersion=0, ///<�汾����Ϊ0�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
        );

	///pack inc����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
	static int packRecvInc(CwxPackageWriter* writer, ///<����pack��writer
		CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
		CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CWX_INT64   num, ///<inc�����֣������ɸ�
        CWX_INT64   max=0, ///<��incΪ��ֵ����ͨ��max�޶����ֵ
        CWX_INT64   min=0, ///<��incΪ��ֵ����ͨ��min�޶���Сֵ
        CWX_UINT32  uiExpire=0, ///<��ʱʱ�䣬��Ϊ0�����
        CWX_UINT32  uiSign=0, ///<��ǣ���Ϊ0�����
        CWX_UINT32  uiVersion=0, ///<�汾����Ϊ0�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
		);

	///����inc�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
	static int parseRecvInc(CwxPackageReader* reader,///<reader
        CwxKeyValueItem const*& key, ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        CWX_INT64&   num, ///<����inc��num
        CWX_INT64&   max, ///<����max
        CWX_INT64&   min, ///<����min
        CWX_UINT32& uiExpire, ///<����expire����Ϊ0��ʾû��ָ��
        CWX_UINT32&  uiSign, ///<����sign
        CWX_UINT32&  uiVersion, ///<����version
        char const*& user,  ///<����user
        char const*& passwd, ///<����password
		char* szErr2K=NULL  ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack delete�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvDel(CwxPackageWriter* writer,///<����pack��writer
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack delete����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvDel(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        CWX_UINT32 uiVersion=0, ///<�汾����Ϊ0�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        char* szErr2K=NULL       ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse delete�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvDel(CwxPackageReader* reader, ///<reader
        CwxKeyValueItem const*& key,   ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        CWX_UINT32& uiVersion, ///<���ذ汾
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
        char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack��inc������ݸ��·�����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvReply(CwxPackageWriter* writer,///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
		CWX_UINT16 unMsgType, ///<�ظ���Ϣ������Ϣ����
        int ret,  ///<���ص�ret����
        CWX_UINT32 uiVersion, ///<���صİ汾��
        CWX_UINT32 uiFieldNum, ///<���ص�field����
        char const* szErrMsg, ///<���صĴ�����Ϣ
        char* szErr2K=NULL    ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse��inc������ݸ��·�����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvReply(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ص����ݰ�
        int& ret,  ///<���ص�retֵ
        CWX_UINT32& uiVersion, ///<���ص�version
        CWX_UINT32& uiFieldNum,  ///<���ص�field number
        char const*& szErrMsg,  ///<���صĴ�����Ϣ
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack inc�ķ�����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvIncReply(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT16 unMsgType, ///<��Ϣ����
        int ret,  ///<ret����
        CWX_INT64 llNum, ///<��������ֵ
        CWX_UINT32 uiVersion, ///<�汾��
        char const* szErrMsg, ///<������Ϣ
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse inc���ص���Ϣ���� ����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvIncReply(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ص����ݰ�
        int& ret,  ///<���ص�retֵ
        CWX_UINT32& uiVersion, ///<���صİ汾
        CWX_INT64& llNum, ///<���صļ�������ֵ
        char const*& szErrMsg, ///<������Ϣ
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///����Ļظ���������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packErrReply(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<��Ϣ��
        CWX_UINT32 uiTaskId, ///<��Ϣ��taskid
        CWX_UINT16 unMsgType, ///<��Ϣ����
        int ret,  ///<���ش���
        char const* szErrMsg, ///<������Ϣ
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack get�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packGetKey(CwxPackageWriter* writer, ///<����pack��writer
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        bool bVersion = false, ///<�Ƿ��ȡ�汾
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        CWX_UINT8 ucKeyInfo=0, ///<�Ƿ��ȡkey��infomation
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack get����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packGetKey(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        bool bVersion = false, ///<�Ƿ��ȡ�汾
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        CWX_UINT8 ucKeyInfo=0, ///<�Ƿ��ȡkey��infomation
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse get�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseGetKey(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CwxKeyValueItem const*& key,   ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        bool&        bVersion, ///<�汾
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
        bool&        bMaster,  ///<��master��ȡ��Ϣ
        CWX_UINT8& ucKeyInfo, ///<�Ƿ��ȡkey��infomation
        char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack exist�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExistKey(CwxPackageWriter* writer, ///<����pack��writer
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        bool bVersion = false, ///<�Ƿ��ȡ�汾
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack exist����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExistKey(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        bool bVersion = false, ///<�Ƿ��ȡ�汾
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse exist�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseExistKey(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CwxKeyValueItem const*& key,   ///<����key�ֶ�
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        bool&        bVersion, ///<�汾
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
        bool&        bMaster,  ///<��master��ȡ��Ϣ
        char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack multi-get���ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packGetKeys(CwxPackageWriter* writer, ///<����pack��writer
        CwxPackageWriter* writer1, ///<����pack��writer1
        list<pair<char const*, CWX_UINT16> > const& keys, ///<key���б�
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        CWX_UINT8 ucKeyInfo=0, ///<�Ƿ��ȡkey��infomation
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack multi-get��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packGetKeys(CwxPackageWriter* writer,///<����pack��writer
        CwxPackageWriter* writer1, ///<����pack��writer1
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        list<pair<char const*, CWX_UINT16> > const& keys, ///<key���б�
        CwxKeyValueItem const* field, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra, ///<extra��Ϣ����ΪNULL�����
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        CWX_UINT8 ucKeyInfo=0, ///<�Ƿ��ȡkey��infomation
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse multi-get�����ݰ��� ����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseGetKeys(CwxPackageReader* reader,///<reader
        CwxPackageReader* reader1,///<reader1
        CwxMsgBlock const* msg, ///<���ݰ�
        list<pair<char const*, CWX_UINT16> >& keys,///<key���б�
        CWX_UINT32& uiKeyNum, ///<key������
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        char const*& user,     ///<�����û���NULL��ʾ������
        char const*& passwd,   ///<���ؿ��NULL��ʾ������
        bool&        bMaster,  ///<��master��ȡ��Ϣ
        CWX_UINT8&   ucKeyInfo, ///<�Ƿ��ȡkey��infomation
        char* szErr2K=NULL     ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack ��ȡkey�б�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packGetList(CwxPackageWriter* writer,///<����pack��writer
        CwxKeyValueItem const* begin=NULL, ///<��ʼ��key
        CwxKeyValueItem const* end=NULL,  ///<������key
        CWX_UINT16  num=0,  ///<���ص�����
        CwxKeyValueItem const* field=NULL, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra=NULL, ///<extra��Ϣ����ΪNULL�����
        bool        bAsc=true, ///<�Ƿ�����
        bool        bBegin=true, ///<�Ƿ��ȡbegin��ֵ
        bool        bKeyInfo=false, ///<�Ƿ񷵻�key��info
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack ��ȡkey�б����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packGetList(CwxPackageWriter* writer,
        CwxMsgBlock*& msg,
        CWX_UINT32 uiTaskId,
        CwxKeyValueItem const* begin=NULL, ///<��ʼ��key
        CwxKeyValueItem const* end=NULL,  ///<������key
        CWX_UINT16  num=0,  ///<���ص�����
        CwxKeyValueItem const* field=NULL, ///<field�ֶΣ���ΪNULL�Ĳ����
        CwxKeyValueItem const* extra=NULL, ///<extra��Ϣ����ΪNULL�����
        bool        bAsc=true, ///<�Ƿ�����
        bool        bBegin=true, ///<�Ƿ��ȡbegin��ֵ
        bool        bKeyInfo=false, ///<�Ƿ񷵻�key��info
        char const* user=NULL,  ///<�û�����ΪNULL�����
        char const* passwd=NULL, ///<�û������ΪNULL�����
        bool bMaster = false, ///<�Ƿ��master��ȡ
        char* szErr2K=NULL   ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse get list�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseGetList(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///���ݰ�
        CwxKeyValueItem const*& begin, ///<���ؿ�ʼ
        CwxKeyValueItem const*& end, ///<���ؼ���
        CWX_UINT16&  num, ///<��ȡ������
        CwxKeyValueItem const*& field, ///<field�ֶΣ���ΪNULL��ʾ������
        CwxKeyValueItem const*& extra, ///<extra��Ϣ����ΪNULL��ʾ������
        bool&        bAsc, ///<����
        bool&        bBegin, ///<�Ƿ��ȡ��ʼֵ
        bool&        bKeyInfo, ///<�Ƿ񷵻�key��info
        char const*& szUser, ///<�û�
        char const*& szPasswd, ///<����
        bool& bMaster, ///<�Ƿ��master��ȡ
        char*        szErr2K=NULL ///<����Ĵ�����Ϣ
        );

    ///pack��Ȩ��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvAuth(CwxPackageWriter* writer,///<����pack��writer
        CwxMsgBlock*& msg,///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId,///<��Ϣ����task id
        char const* szUser = NULL, ///<�û�����ΪNULL�����
        char const* szPasswd = NULL,///<�û������ΪNULL�����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse��Ȩ�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvAuth(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg,///<���ݰ�
        char const*& szUser,///<�����û���NULL��ʾ������
        char const*& szPasswd,///<���ؿ��NULL��ʾ������
        char*     szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack��Ȩ�ظ�����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packRecvAuthReply(CwxPackageWriter* writer,///<����pack��writer
        CwxMsgBlock*& msg,///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId,///<��Ϣ����task id
        CWX_UINT16 unMsgType, ///<��Ϣ����
        int ret, ///<��Ȩ���
        char const* szErrMsg, ///<������Ϣ
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse��Ȩ�ظ������ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseRecvAuthReply(CwxPackageReader* reader,///<reader
        CwxMsgBlock const* msg,///<���ݰ�
        int& ret,///<��Ȩ���
        char const*& szErrMsg,///<������Ϣ
        char* szErr2K=NULL///<���ʱ�Ĵ�����Ϣ
        );

    ///pack export��report��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExportReport(CwxPackageWriter* writer,///<����pack��writer
        CwxMsgBlock*& msg,///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId,///<��Ϣ����task id
        CWX_UINT32  uiChunkSize, ///<���ݷ��͵�chunk��С
        char const* subscribe = NULL, ///<���ݶ�������
        char const* key = NULL, ///<��ʼ��key
        char const* extra = NULL, ///<extra��Ϣ����ΪNULL�����
        char const* user=NULL, ///<�û���
        char const* passwd=NULL, ///<����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse export��report���ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseExportReport(CwxPackageReader* reader,///<reader
        CwxMsgBlock const* msg,///<���ݰ�
        CWX_UINT32&  uiChunkSize,///<���ݷ��͵�chunk��С
        char const*& subscribe,///<���ݶ����������ձ�ʾȫ������
        char const*& key,///<��ʼ��key���ձ�ʾû������
        char const*& extra, ///<extra��Ϣ����ΪNULL��ʾû��ָ��
        char const*& user,///<�û���
        char const*& passwd,///<����
        char* szErr2K=NULL///<���ʱ�Ĵ�����Ϣ
        );

    ///pack export��report�ظ���Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExportReportReply(CwxPackageWriter* writer,///<����pack��writer
        CwxMsgBlock*& msg,///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId,///<��Ϣ����task id
        CWX_UINT64 ullSession, ///<session
        CWX_UINT64 ullSid,  ///<���ݿ�ʼ����ʱ��sid
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse export��report�ظ���Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseExportReportReply(CwxPackageReader* reader,///<reader
        CwxMsgBlock const* msg,///<���ݰ�
        CWX_UINT64& ullSession,///<session
        CWX_UINT64& ullSid,///<���ݿ�ʼ����ʱ��sid
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///packһ��export��key/value�����ݡ�����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExportDataItem(CwxPackageWriter* writer,///<����pack��writer
        CwxKeyValueItem const& key, ///<key
        CwxKeyValueItem const& data, ///<data
        CwxKeyValueItem const* extra, ///<extra
        CWX_UINT32 version, ///<�汾��
        CWX_UINT32 expire, ///<��ʱʱ��
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parseһ��export��key/value�����ݷ���ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseExportDataItem(CwxPackageReader* reader,///<reader
        char const* szData, ///<key/value����
        CWX_UINT32  uiDataLen, ///<key/value���ݵĳ���
        CwxKeyValueItem const*& key, ///<���ݵ�key
        CwxKeyValueItem const*& data, ///<���ݵ�data
        CwxKeyValueItem const*& extra, ///<extra
        CWX_UINT32& version, ///<���ݵİ汾
        CWX_UINT32& expire, ///<���ݵĳ�ʱ
        char* szErr2K=NULL///<���ʱ�Ĵ�����Ϣ
        );

    ///pack��chunk��֯�Ķ���export��key/value����Ϣ������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packMultiExportData(
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        char const* szData,  ///<����key/value��ɵ�����package
        CWX_UINT32 uiDataLen, ///<���ݵĳ���
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT64 ullSeq, ///<���к�
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse��chunk��֯�Ķ���export��key/value�����ݡ�����ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseMultiExportData(
        CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg,///<���ݰ�
        CWX_UINT64& ullSeq, ///<���к�
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack export���ݵ�reply��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExportDataReply(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSeq, ///<���к�
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse export���ݵ�reply��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseExportDataReply(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSeq, ///<���к�
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack export��ɵ���Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packExportEnd(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSid, ///<���ʱ��sid
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse export��ɵ���Ϣ������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseExportEnd(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSid,///<���ʱ��sid
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack binlog sync��report��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packReportData(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSid, ///<��ʼ��sid
        bool      bNewly,  ///<�Ƿ������binlog��ʼͬ��
        CWX_UINT32  uiChunkSize, ///<ͬ����chunk��С
        char const* subscribe = NULL, ///<binlog���Ĺ��򣬿ձ�ʾȫ������
        char const* user=NULL, ///<�û���
        char const* passwd=NULL, ///<�û�����
        char const* sign=NULL, ///<ǩ����ʽ���ձ�ʾ��ǩ��
        bool        zip = false, ///<�Ƿ�ѹ��
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse binlog sync��report��Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseReportData(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSid, ///<��ʼ��sid
        bool&       bNewly, ///<�Ƿ������binlog��ʼͬ��
        CWX_UINT32&  uiChunkSize, ///<ͬ����chunk��С
        char const*& subscribe, ///<binlog���Ĺ��򣬿ձ�ʾȫ������
        char const*& user, ///<�û���
        char const*& passwd, ///<�û�����
        char const*& sign, ///<ǩ����ʽ���ձ�ʾ��ǩ��
        bool&        zip, ///<�Ƿ�ѹ��
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack report�Ļظ���Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packReportDataReply(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSession, ///<session id
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse report�Ļظ����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseReportDataReply(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSession, ///<session id
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack sync��session���ӱ�����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packReportNewConn(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSession, ///<����������session
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse sync��session���ӱ������ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseReportNewConn(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSession, ///<����������session
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack report��sync�ĳ�����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packSyncErr(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        int ret, ///<�������
        char const* szErrMsg, ///<������Ϣ
        char* szErr2K=NULL///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse report��sync�ĳ������ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseSyncErr(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        int& ret,  ///<�������
        char const*& szErrMsg,  ///<������Ϣ
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );


    ///pack sync��һ��binlog����Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packSyncData(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSid, ///<binlog��sid
        CWX_UINT32 uiTimeStamp, ///<binlog��ʱ���
        CwxKeyValueItem const& data, ///<binlog��data
        CWX_UINT32 group,  ///<binlog�����ķ���
        CWX_UINT32 type,   ///<binlog�����ͣ�Ҳ������Ϣ����
        CWX_UINT32 version,  ///<��Ӧ��key�İ汾
        CWX_UINT64 ullSeq,  ///<��Ϣ�����к�
        char const* sign=NULL, ///<ǩ����ʽ
        bool       zip = false, ///<�Ƿ�ѹ��
        char* szErr2K=NULL///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack һ��binlog�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packSyncDataItem(CwxPackageWriter* writer, ///<����pack��writer
        CWX_UINT64 ullSid, ///<binlog��sid
        CWX_UINT32 uiTimeStamp, ///<binlog��ʱ���
        CwxKeyValueItem const& data, ///<binlog��data
        CWX_UINT32 group,  ///<binlog�����ķ���
        CWX_UINT32 type,   ///<binlog�����ͣ�Ҳ������Ϣ����
        CWX_UINT32 version,  ///<��Ӧ��key�İ汾
        char const* sign=NULL, ///<ǩ����ʽ
        char* szErr2K=NULL///<pack����ʱ�Ĵ�����Ϣ
        );

    ///pack ����binlog�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packMultiSyncData(
        CWX_UINT32 uiTaskId, ///<����id
        char const* szData, ///<������Ϣ������buf
        CWX_UINT32 uiDataLen, ///<�������ݵ�����buf����
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT64 ullSeq, ///<��Ϣ������Ϣ���к�
        bool  zip = false, ///<�Ƿ�ѹ��
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parseһ��binlog�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseSyncData(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSid, ///<binlog��sid
        CWX_UINT32& uiTimeStamp, ///<binlog��ʱ���
        CwxKeyValueItem const*& data, ///<binlog������
        CWX_UINT32& group, ///<binlog������group
        CWX_UINT32& type, ///<binlog��Ӧ�����ݱ����Ϣ����
        CWX_UINT32& version, ///<binlog��Ӧ�����ݱ����key�İ汾
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///parseһ��binlog�����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseSyncData(CwxPackageReader* reader,
        char const* szData,
        CWX_UINT32 uiDataLen,
        CWX_UINT64& ullSid, ///<binlog��sid
        CWX_UINT32& uiTimeStamp, ///<binlog��ʱ���
        CwxKeyValueItem const*& data, ///<binlog������
        CWX_UINT32& group, ///<binlog������group
        CWX_UINT32& type, ///<binlog��Ӧ�����ݱ����Ϣ����
        CWX_UINT32& version, ///<binlog��Ӧ�����ݱ����key�İ汾
        char* szErr2K=NULL ///<���ʱ�Ĵ�����Ϣ
        );

    ///pack sync binlog�Ļظ���Ϣ��������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int packSyncDataReply(CwxPackageWriter* writer, ///<����pack��writer
        CwxMsgBlock*& msg, ///<���ص���Ϣ�����������ڲ�����
        CWX_UINT32 uiTaskId, ///<��Ϣ����task id
        CWX_UINT64 ullSeq, ///<��Ϣ�����к�
        CWX_UINT16 unMsgType, ///<��Ϣ����
        char* szErr2K=NULL ///<pack����ʱ�Ĵ�����Ϣ
        );

    ///parse sync binlog�Ļظ����ݰ�������ֵ��UNISTOR_ERR_SUCCESS���ɹ�����������ʧ��
    static int parseSyncDataReply(CwxPackageReader* reader, ///<reader
        CwxMsgBlock const* msg, ///<���ݰ�
        CWX_UINT64& ullSeq, ///<��Ϣ�����к�
        char* szErr2K=NULL  ///<���ʱ�Ĵ�����Ϣ
        );

    ///��������ͬ������seq��
    inline static void setSeq(char* szBuf, CWX_UINT64 ullSeq){
        CWX_UINT32 byte4 = (CWX_UINT32)(ullSeq>>32);
        byte4 = CWX_HTONL(byte4);
        memcpy(szBuf, &byte4, 4);
        byte4 = (CWX_UINT32)(ullSeq&0xFFFFFFFF);
        byte4 = CWX_HTONL(byte4);
        memcpy(szBuf + 4, &byte4, 4);

    }    
    ///��ȡ����ͬ������seq��
    inline static CWX_UINT64 getSeq(char const* szBuf) {
        CWX_UINT64 ullSeq = 0;
        CWX_UINT32 byte4;
        memcpy(&byte4, szBuf, 4);
        ullSeq = CWX_NTOHL(byte4);
        memcpy(&byte4, szBuf+4, 4);
        ullSeq <<=32;
        ullSeq += CWX_NTOHL(byte4);
        return ullSeq;
    }

    ///�Ƿ�������Ҷ��ĵ���Ϣ����
    inline static bool isContinueSeek(CWX_UINT32 uiSeekedNum){
        return MAX_CONTINUE_SEEK_NUM>uiSeekedNum;
    }
private:
    ///��ֹ��������ʵ��
    UnistorPoco()
    {
    }
    ///��������
    ~UnistorPoco();
};





#endif
