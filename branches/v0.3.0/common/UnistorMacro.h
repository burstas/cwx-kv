#ifndef __UNISTOR_MACRO_H__
#define __UNISTOR_MACRO_H__


#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"

CWINUX_USING_NAMESPACE


///ͨ�ŵ�key����
#define UNISTOR_KEY_ASC    "asc"   ///<����
#define UNISTOR_KEY_BEGIN      "begin" ///<��ʼ 
#define UNISTOR_KEY_C       "c"  ///<cache
#define UNISTOR_KEY_CHUNK "chunk"  ///<���ݷ��͵�chunk��key
#define UNISTOR_KEY_CRC32  "crc32" ///<crc32ǩ��
#define UNISTOR_KEY_END    "end"   ///<����ֵ��key
#define UNISTOR_KEY_ERR  "err"     ///error��key
#define UNISTOR_KEY_E "e"          ///<expire��key
#define UNISTOR_KEY_D "d"           ///<���ݵ�data��key
#define UNISTOR_KEY_F  "f"          ///<field
#define UNISTOR_KEY_FN "fn"         ///<field number
#define UNISTOR_KEY_G   "g"         ///<group,Ϊkey�ķ���
#define UNISTOR_KEY_I "i"           ///<information
#define UNISTOR_KEY_IB "ib"         ///<isbegin
#define UNISTOR_KEY_K "k"           ///<���ݵ�key��key
#define UNISTOR_KEY_M      "m"      ///<message��key
#define UNISTOR_KEY_MT "mt"         ///<master
#define UNISTOR_KEY_MD5    "md5"    ///<md5ǩ����key
#define UNISTOR_KEY_MAX    "max"    ///<���ֵ
#define UNISTOR_KEY_MIN    "min"    ///<��Сֵ
#define UNISTOR_KEY_N "n"           ///<number
#define UNISTOR_KEY_P "p"            ///<password
#define UNISTOR_KEY_RET  "ret"       ///<����ֵ��ret
#define UNISTOR_KEY_SEQ   "seq"      ///<����ͬ������Ϣ���к�
#define UNISTOR_KEY_SESSION "session" ///<����ͬ����session
#define UNISTOR_KEY_SID "sid"        ///<���ݱ����sid
#define UNISTOR_KEY_SIGN   "sign"    ///<���ݸ��µ�sign
#define UNISTOR_KEY_SUBSCRIBE  "subscribe" ///<���ĵ�key
#define UNISTOR_KEY_T "t"           ///<timestamp
#define UNISTOR_KEY_TYPE   "type"   ///<binlog�����ͣ�Ҳ�������ݸ��µ���Ϣ����
#define UNISTOR_KEY_U    "u"       ///<user
#define UNISTOR_KEY_ZIP    "zip"  ///<ѹ����ʾ
#define UNISTOR_KEY_V      "v"   ///<version key
#define UNISTOR_KEY_X      "x"  ///<������չkey

///�����Ķ��󴴽�symbol name
#define UNISTOR_ENGINE_CREATE_SYMBOL_NAME  "unistor_create_engine"

///������붨�壬0~100��ϵͳ���Ĵ���
#define UNISTOR_ERR_SUCCESS          0  ///<�ɹ�
#define UNISTOR_ERR_ERROR            1 ///<�������ֵ���ͳ����
#define UNISTOR_ERR_FAIL_AUTH        2 ///<��Ȩʧ��
#define UNISTOR_ERR_LOST_SYNC        3 ///<ʧȥ��ͬ��״̬
#define UNISTOR_ERR_NO_MASTER        4 ///<û��master

//100������Ӧ�ü��Ĵ���
#define UNISTOR_ERR_NEXIST            101   ///<������
#define UNISTOR_ERR_EXIST             102   ///<����
#define UNISTOR_ERR_VERSION           103   ///<�汾����
#define UNISTOR_ERR_OUTRANGE          104   ///<inc������Χ
#define UNISTOR_ERR_TIMEOUT           105   ///<��ʱ

#define UNISTOR_TRANS_TIMEOUT_SECOND   5  ///<����ת����ʱ
#define UNISTOR_CONN_TIMEOUT_SECOND    3  ///<���ӳ�ʱ

#define UNISTOR_CLOCK_INTERANL_SECOND  1  ///<ʱ�Ӽ��
#define UNISTOR_CHECK_ZK_LOCK_EXPIRE   10  ///<���zk��״����ʱ

#define UNISTOR_MAX_DATA_SIZE              4 * 1024 * 1024 ///<����DATA��С
#define UNISTOR_MAX_KEY_SIZE               1024            ///<����key�Ĵ�С
#define UNISTOR_MAX_KV_SIZE				  (4 * 1024 * 1024 +  8 * 1024) ///<����data size
#define UNISTOR_DEF_KV_SIZE                (64 * 1024) ///<ȱʡ�Ĵ洢��С
#define UNISTOR_MAX_KVS_SIZE              (20 * 1024 * 1024) ///<��󷵻����ݰ���СΪ20M
#define UNISTOR_MAX_CHUNK_KSIZE         (20 * 1024) ///<����chunk size
#define UNISTOR_ZIP_EXTRA_BUF           128
#define UNISTOR_DEF_LIST_NUM             50    ///<list��ȱʡ����
#define UNISTOR_MAX_LIST_NUM             1000   ///<list���������
#define UNISTOR_MAX_BINLOG_FLUSH_COUNT  10000 ///<��������ʱ������skip sid����
#define UNISTOR_KEY_START_VERION        1 ///<key����ʼ�汾��
#define UNISTOR_MAX_GETS_KEY_NUM        1024 ///<����mget��key������

#define  UNISTOR_WRITE_CACHE_MBYTE          128  ///<128M��дcache
#define  UNISTOR_WRITE_CACHE_KEY_NUM        10000 ///<cache������¼��

#define  UNISTOR_DEF_SOCK_BUF_KB  64   ///<ȱʡ��socket buf����λΪKB
#define  UNISTOR_MIN_SOCK_BUF_KB  4    ///<��С��socket buf����λΪKB
#define  UNISTOR_MAX_SOCK_BUF_KB  (8 * 1024) ///<����socket buf����λΪKB
#define  UNISTOR_DEF_CHUNK_SIZE_KB 64   ///<ȱʡ��chunk��С����λΪKB
#define  UNISTOR_MIN_CHUNK_SIZE_KB  4   ///<��С��chunk��С����λΪKB
#define  UNISTOR_MAX_CHUNK_SIZE_KB  UNISTOR_MAX_CHUNK_KSIZE ///<����chunk��С����λΪKB
#define  UNISTOR_DEF_STORE_FLUSH_NUM  1000  ///<unistor�洢����flush��ȱʡ���ݱ������
#define  UNISTOR_MIN_STORE_FLUSH_NUM  1     ///<unistor�洢����flush����С���ݱ������
#define  UNISTOR_MAX_STORE_FLUSH_NUM  500000 ///<unistor�洢����flush��������ݱ������
#define  UNISTOR_DEF_CONN_NUM  10            ///<����ͬ��ȱʡ�Ĳ�����������
#define  UNISTOR_MIN_CONN_NUM  1            ///<����ͬ����С�Ĳ�����������
#define  UNISTOR_MAX_CONN_NUM  256          ///<����ͬ�����Ĳ����������� 
#define  UNISTOR_DEF_EXPIRE_CONNCURRENT 32  ///<��ʱ����ȱʡ������Ϣ����
#define  UNISTOR_MIN_EXPIRE_CONNCURRENT 1   ///<��ʱ������С������Ϣ����
#define  UNISTOR_MAX_EXPIRE_CONNCURRENT 256 ///<��ʱ������󲢷���Ϣ����
#define  UNISTOR_CHECKOUT_INTERNAL   5      ///<�洢�����checkpoint�ļ��
#define  UNISTOR_PER_FETCH_EXPIRE_KEY_NUM 1024  ///<��ʱ����һ�λ�ȡkey������
#define  UNISTOR_EXPORT_CONTINUE_SEEK_NUM  4096  ///<���ݵ���һ�α�����key������


#define  UNISTOR_REPORT_TIMEOUT  30  ///<report�ĳ�ʱʱ��
#define  UNISTOR_TRAN_AUTH_TIMEOUT 30  ///<ת����֤�ĳ�ʱʱ��

#define  UNISTOR_MASTER_SWITCH_SID_INC  1000000  ///<unistor����master�л�����master��Ծ��sid��ֵ




///��ʼд�ĺ���������ֵ��0���ɹ���-1��ʧ��
typedef int (*UNISTOR_WRITE_CACHE_WRITE_BEGIN_FN)(void* context, char* szErr2K);
///д���ݣ�����ֵ��0���ɹ���-1��ʧ��
typedef int (*UNISTOR_WRITE_CACHE_WRITE_WRITE_FN)(void* context, char const* szKey, CWX_UINT16 unKeyLen, char const* szData, CWX_UINT32 uiDataLen, bool bDel, CWX_UINT32 ttOldExpire, char* szStoreKeyBuf, CWX_UINT16 unKeyBufLen, char* szErr2K);
///�ύ���ݣ�����ֵ��0���ɹ���-1��ʧ��
typedef int (*UNISTOR_WRITE_CACHE_WRITE_END_FN)(void* context, CWX_UINT64 ullSid, char* szErr2K);
///key����ȱȽϺ�����true����ȣ�false�������
typedef bool (*UNISTOR_KEY_CMP_EQUAL_FN)(char const* key1, CWX_UINT16 unKey1Len, char const* key2, CWX_UINT16 unKey2Len);
///key��less�ȽϺ�����0����ȣ�-1��С�ڣ�1������
typedef int (*UNISTOR_KEY_CMP_LESS_FN)(char const* key1, CWX_UINT16 unKey1Len, char const* key2, CWX_UINT16 unKey2Len);
///key��hash������
typedef size_t (*UNISTOR_KEY_HASH_FN)(char const* key1, CWX_UINT16 unKey1Len);
///key��group������
typedef CWX_UINT32 (*UNISTOR_KEY_GROUP_FN)(char const* key1, CWX_UINT16 unKey1Len);
///key��ascii����less������0����ȣ�-1��С�ڣ�1������
typedef int (*UNISTOR_KEY_ASCII_LESS)(char const* key1, CWX_UINT16 unKey1Len, char const* key2, CWX_UINT16 unKey2Len);

#endif
