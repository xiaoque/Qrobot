

//
//	WaveAPI.cpp
//
// by: 知秋一叶 
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <aviriff.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define BUFFER_SIZE 1024*8             // 录制声音长度
typedef int SR_DWORD;
typedef short int SR_WORD ;

bool m_record ,first_write;

static int buf_count = 0;
static int dwDataLength  = 0;
PBYTE pSaveBuffer ,pNewBuffer;   // buffer address 
// 函数定义
void CALLBACK waveInProc(HWAVEIN hwi,      
						 UINT uMsg,         
						 DWORD_PTR dwInstance,  
						 DWORD_PTR dwParam1,  
						 DWORD_PTR dwParam2     );
void CALLBACK waveOutProc(  HWAVEOUT hwo,   
						  UINT uMsg,         
						  DWORD_PTR dwInstance,   
						  DWORD_PTR dwParam1,    
						  DWORD_PTR dwParam2     );

struct wave_pcm_hdr
{
	char            riff[4];                        // = "RIFF"
	SR_DWORD        size_8;                         // = FileSize - 8
	char            wave[4];                        // = "WAVE"
	char            fmt[4];                         // = "fmt "
	SR_DWORD        dwFmtSize;                      // = 下一个结构体的大小 : 16

	SR_WORD         format_tag;              // = PCM : 1
	SR_WORD         channels;                 // = 通道数 : 1
	SR_DWORD        samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	SR_DWORD        avg_bytes_per_sec;      // = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
	SR_WORD         block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	SR_WORD         bits_per_sample;         // = 量化比特数: 8 | 16

	char            data[4];                        // = "data";
	SR_DWORD        data_size;                // = 纯数据长度 : FileSize - 44 
} ;
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
// 入口
int main()
{
	/* 录音 */
	HWAVEIN hWaveIn;   //input handle
   WAVEFORMATEX waveform;//wave form
	WAVEINCAPS wic;        // input device
	PBYTE pBuffer1 ,pBuffer2;      // input buffer


	HWAVEOUT hWaveOut;     //ouput handle
	PWAVEHDR pWaveHdr1 ,pWaveHdr2 ;
	WAVEHDR wavhdr;      //ouput data buffer


	pWaveHdr1=reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	pWaveHdr2=reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR))); //给声音文件头分配内存空间
	pSaveBuffer = reinterpret_cast<PBYTE>(malloc(1));    //给数据内存地址分配空间

	// Device
	int nReturn = waveInGetNumDevs();
	printf("输入设备数目：%d\n", nReturn);
	for (int i=0; i<nReturn; i++)
	{
		waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS));
		printf("#%d\t设备名：%s\n", i, wic.szPname);
	}

	// open

	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nChannels = 1;
	waveform.nSamplesPerSec = 16000;
	waveform.nAvgBytesPerSec = waveform.nSamplesPerSec * sizeof(unsigned short); 
	waveform.nBlockAlign = waveform.nChannels * waveform.wBitsPerSample / 8;
	waveform.wBitsPerSample = 16;
	waveform.cbSize = 0;

	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)waveInProc, NULL, CALLBACK_FUNCTION);


	waveInGetDevCaps((UINT_PTR)hWaveIn, &wic, sizeof(WAVEINCAPS));
	printf("打开的输入设备：%s\n", wic.szPname);

	// prepare buffer
	m_record=true;
	pBuffer1=(PBYTE)malloc(BUFFER_SIZE);
	pBuffer2=(PBYTE)malloc(BUFFER_SIZE);    //给缓冲区分配空间
	if (!pBuffer1||!pBuffer2)
	{
		if (pBuffer1) free(pBuffer1);
		if (pBuffer2) free(pBuffer2);
		printf("memory error");
		return 0;
	}

	//初始化声音文件头
	pWaveHdr1->lpData=(LPSTR)pBuffer1;   //设置缓冲区
	pWaveHdr1->dwBufferLength=BUFFER_SIZE; //缓冲区大小
	pWaveHdr1->dwBytesRecorded=0;
	pWaveHdr1->dwUser=0;
	pWaveHdr1->dwFlags=0;
	pWaveHdr1->dwLoops=1;
	pWaveHdr1->lpNext=NULL;
	pWaveHdr1->reserved=0;

   
	pWaveHdr2->lpData=(LPSTR)pBuffer2;
	pWaveHdr2->dwBufferLength=BUFFER_SIZE;
	pWaveHdr2->dwBytesRecorded=0;
	pWaveHdr2->dwUser=0;
	pWaveHdr2->dwFlags=0;
	pWaveHdr2->dwLoops=1;
	pWaveHdr2->lpNext=NULL;
	pWaveHdr2->reserved=0;

  
	pSaveBuffer = (PBYTE)realloc (pSaveBuffer, 1) ;



	// record
	printf ("Press any key to start recording...\n");
    getch();
	waveInPrepareHeader(hWaveIn,pWaveHdr1,sizeof(WAVEHDR));  //将缓冲区信息和输入设备关联
	waveInAddBuffer(hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ; //将缓冲区地址添加到输入设备中
	waveInPrepareHeader(hWaveIn,pWaveHdr2,sizeof(WAVEHDR));
	waveInAddBuffer (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ; //d...\n");
	printf("Start Record!\n\n");

	buf_count = 0;
	m_record = true;
	waveInStart(hWaveIn);


	printf ("Press any key to stop recording...\n");
    getch();
	
	printf("Record Over!\n\n");/*
	if(buf_count==0)
	{
		FILE * fp = fopen("E:\\ecord.pcm","ab+");
		if(fp==NULL)
		{
			printf("file open error");
		}
		
		pNewBuffer = (PBYTE)realloc (pSaveBuffer, dwDataLength + pWaveHdr1->dwBytesRecorded) ;
		pSaveBuffer = pNewBuffer;
		CopyMemory(pSaveBuffer + dwDataLength, pWaveHdr1->lpData,pWaveHdr1->dwBytesRecorded);
		dwDataLength += pWaveHdr1->dwBytesRecorded;

		pNewBuffer = (PBYTE)realloc (pSaveBuffer, dwDataLength + pWaveHdr2->dwBytesRecorded) ;
		pSaveBuffer = pNewBuffer;
		CopyMemory(pSaveBuffer + dwDataLength, pWaveHdr2->lpData,pWaveHdr2->dwBytesRecorded);
		dwDataLength += pWaveHdr2->dwBytesRecorded;

		fwrite(pWaveHdr1->lpData,pWaveHdr1->dwBytesRecorded,1,fp);
		fwrite(pWaveHdr2->lpData,pWaveHdr2->dwBytesRecorded,1,fp);

		fclose(fp);
	}
	*/
	free(pBuffer1);
	free(pBuffer2);

	// clean
	waveInStop(hWaveIn);
	//waveInReset(hWaveIn);
//_record  = false;
	waveInClose(hWaveIn);

	system("pause");
	printf("\n");


	return 0;
}

// 录音回调函数
void CALLBACK waveInProc(HWAVEIN hwi,      
						 UINT uMsg,         
						 DWORD_PTR dwInstance,  
						 DWORD_PTR dwParam1,  
						 DWORD_PTR dwParam2     )
{
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;

	if ((WIM_DATA==uMsg) && (buf_count<BUFFER_SIZE))
	{
		pNewBuffer = (PBYTE)realloc (pSaveBuffer, dwDataLength + pwh->dwBytesRecorded) ; 
		if(pNewBuffer ==NULL)
		{
			waveInClose(hwi);
			printf("memory error");
			return;
		}
		pSaveBuffer = pNewBuffer;
		buf_count ++;
		CopyMemory(pSaveBuffer + dwDataLength, pwh->lpData,pwh->dwBytesRecorded);
		dwDataLength += pwh->dwBytesRecorded;

		if(!m_record)
		{
			waveInClose(hwi);
			printf("stop recording");
			return;
		}

		FILE * fp = fopen("E:\\ecord.pcm","ab+");
		if(fp==NULL)
		{
			printf("file open error");
		}
		fwrite(pwh->lpData,pwh->dwBytesRecorded,1,fp);
		fclose(fp);
				
		waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));
	}
}

