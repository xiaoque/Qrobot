
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <MMSystem.h>

#include "qtts.h"

#ifdef _WIN64
#pragma comment(lib,"lib\\msc_x64.lib")//x64
#else
#pragma comment(lib,"lib\\msc.lib")//x86
#endif


#define END_SYNTH( reason )           \
{   \
	ret = QTTSSessionEnd( session_id, #reason ); \
	if(MSP_SUCCESS != ret ) \
	{     \
	printf("QTTSSessionEnd failed, error code is %d", ret ); \
	}     \
	\
	ret = QTTSFini();    \
	if(MSP_SUCCESS != ret ) \
	{     \
	printf("QTTSFini failed, error code is %d", ret ); \
	}     \
}

typedef int SR_DWORD;
typedef short int SR_WORD ;


//��Ƶͷ����ʽ
struct wave_pcm_hdr
{
	char            riff[4];                        // = "RIFF"
	SR_DWORD        size_8;                         // = FileSize - 8
	char            wave[4];                        // = "WAVE"
	char            fmt[4];                         // = "fmt "
	SR_DWORD        dwFmtSize;                      // = ��һ���ṹ��Ĵ�С : 16

	SR_WORD         format_tag;              // = PCM : 1
	SR_WORD         channels;                 // = ͨ���� : 1
	SR_DWORD        samples_per_sec;        // = ������ : 8000 | 6000 | 11025 | 16000
	SR_DWORD        avg_bytes_per_sec;      // = ÿ���ֽ��� : dwSamplesPerSec * wBitsPerSample / 8
	SR_WORD         block_align;            // = ÿ�������ֽ��� : wBitsPerSample / 8
	SR_WORD         bits_per_sample;         // = ����������: 8 | 16

	char            data[4];                        // = "data";
	SR_DWORD        data_size;                // = �����ݳ��� : FileSize - 44 
} ;

//Ĭ����Ƶͷ������
struct wave_pcm_hdr default_pcmwavhdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};



int main(void){
	const char*  configs = NULL;
	const char*  session_id = NULL;
	const char*  synth_params = NULL;
	const char*  synth_text = NULL;
	unsigned int text_len = 0;
	const char*  synth_speech = NULL;
	unsigned int synth_speech_len = 0;
	FILE*        f_speech = NULL;
	int          synth_status = 0;
	int          ret = MSP_SUCCESS;
	struct wave_pcm_hdr pcmwavhdr = default_pcmwavhdr;

	printf(
		"=======================\n"
		"    Demo for TTS \n"
		"=======================\n"
		);
	/* ��ʼ�� */
	//server_url=dev.voicecloud.cn/index.htm
	//appid=51b3f1d5
	configs = "appid=51b3f1d5, timeout=10000, coding_libs=speex.dll";
	ret = QTTSInit( configs );
	if(MSP_SUCCESS != ret )
	{
		printf( "QTTSInit failed, error code is %d", ret );
		return -1;
	}
	/* ��ʼһ·�Ự��ʹ�ûỰģʽ */
	synth_params = "ssm=1, auf=audio/L16;rate=16000, aue=speex-wb;7, ent=intp65,vcn=xiaoyan";
	session_id = QTTSSessionBegin( synth_params, &ret );
	if(MSP_SUCCESS != ret )
	{
		printf( "QTTSSessionBegin failed, error code is %d", ret );
		return -1;
	}

	/* д��ϳ��ı� */
	synth_text = "a pig is flying.One day a pig is captured by a angle,so it can fly.";
//	synth_text = "Ѷ��������Ϊ���ṩ��������õ������������飬"
//		"�����ڻ������Ͽ��ſƴ�Ѷ�������з��ĸ�������������"
//		"�����������ȵ������ϳɼ���������ʶ����������ʶ�����ȡ�";
//	synth_text = "���Ƕ��Ǻ����갡���ð������꣡�����ϳ�ʲô��ѽ����ѽ��ȥ����";
	text_len = strlen( synth_text );
	//................
	printf("synth_text:%s\n",synth_text);
	printf("text_len:%d\n",text_len);
	ret = QTTSTextPut( session_id, synth_text, text_len, NULL );
	if(MSP_SUCCESS != ret )
	{
		printf( "QTTSTextPut failed, error code is %d", ret );
		END_SYNTH( QTTSTextPut failed! );
		return -1;
	}
	/* ��ȡ�ϳ���Ƶ */
	f_speech = fopen( "synth_speech.wav", "wb" );
	
	if( NULL == f_speech )
	{
		printf( "Can not open file \"synth_speech.pcm\"" );
		END_SYNTH( open file );
		return -1;
	}
	//.........
	fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), f_speech);
	while( MSP_TTS_FLAG_DATA_END != synth_status )
	{
		printf("SESSION_ID:%s\n",session_id);
		printf("synth_speech_len:%d\n",synth_speech_len);
		printf("synth_status:%d\n",synth_status);
		synth_speech = (char *)QTTSAudioGet( session_id, &synth_speech_len
			, &synth_status, &ret );
		printf("ret:%d\n",ret);
		if(MSP_SUCCESS != ret )
		{
			printf( "QTTSAudioGet failed, error code is: %d", ret );
			break;
		}
		printf("SESSION_ID:%s\n",session_id);
		printf("synth_speech_len:%d\n",synth_speech_len);
		printf("synth_status:%d\n",synth_status);
		printf( "QTTSAudioGet ok, speech length = %d\n", synth_speech_len );
		if( NULL != synth_speech && 0 != synth_speech_len )
		{
			fwrite( synth_speech, 1, synth_speech_len, f_speech );
			pcmwavhdr.data_size += synth_speech_len;//����pcm���ݵĴ�С
		}
	}

	//����pcm�ļ�ͷ���ݵĴ�С
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;

	//��������������д���ļ�ͷ��
	fseek(f_speech, 4, 0);
	fwrite(&pcmwavhdr.size_8,sizeof(pcmwavhdr.size_8), 1, f_speech);
	fseek(f_speech, 40, 0);
	fwrite(&pcmwavhdr.data_size,sizeof(pcmwavhdr.data_size), 1, f_speech);

	fclose( f_speech );

	/* �����Ự���ͷ���Դ */
	END_SYNTH(normal end)
/*	ret = QTTSSessionEnd( session_id, "normal end" );
	if( NULL == f_speech )
	{
		printf( "QTTSSessionEnd failed, error code is %d", ret );
	}
	session_id = NULL;
	ret = QTTSFini();
	if(MSP_SUCCESS != ret )
	{
		printf( "QTTSFini failed, error code is %d", ret );
	}
	*/

	PlaySound(_T("synth_speech.wav"),NULL,SND_FILENAME|SND_ASYNC);
	
	char key = _getch();

	while (true){
		if(key == 'y'){
			break;
		}
		if(key == 'p'){
			PlaySound(NULL,NULL,SND_SYNC);
			break;
		}
		key = _getch();
	}
	MessageBox(NULL,_T("��Ĳ�������"),_T("��������"),MB_OK);
	return 0;


}
