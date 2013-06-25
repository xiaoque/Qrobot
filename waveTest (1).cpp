

//
//	WaveAPI.cpp
//
// by: ֪��һҶ 
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <aviriff.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define BUFFER_SIZE 1024*8             // ¼����������
typedef int SR_DWORD;
typedef short int SR_WORD ;

bool m_record;  //record status
static int buf_count = 0;
static int dwDataLength  = 0;
PBYTE pSaveBuffer ,pNewBuffer;   // buffer address 
// ��������
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
// ���
int main()
{
	/* ¼�� */
	HWAVEIN hWaveIn;              //input handle
	WAVEFORMATEX waveform;        //wave form
	WAVEINCAPS wic;               // input device
	PBYTE pBuffer1 ,pBuffer2;      // input buffer
	PWAVEHDR pWaveHdr1 ,pWaveHdr2 ;// input buffer header


	pWaveHdr1=reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	pWaveHdr2=reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR))); //�������ļ�ͷ�����ڴ�ռ�
	pSaveBuffer = reinterpret_cast<PBYTE>(malloc(1));    //�������ڴ��ַ����ռ�

	m_record = false;
	// Device
	int nReturn = waveInGetNumDevs();
	printf("�����豸��Ŀ��%d\n", nReturn);
	for (int i=0; i<nReturn; i++)
	{
		waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS));
		printf("#%d\t�豸����%s\n", i, wic.szPname);
	}


	//set wave form 
	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nChannels = 1;
	waveform.nSamplesPerSec = 16000;
	waveform.nAvgBytesPerSec = waveform.nSamplesPerSec * sizeof(unsigned short); 
	waveform.nBlockAlign = waveform.nChannels * waveform.wBitsPerSample / 8;
	waveform.wBitsPerSample = 16;
	waveform.cbSize = 0;

	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)waveInProc, NULL, CALLBACK_FUNCTION);
	waveInGetDevCaps((UINT_PTR)hWaveIn, &wic, sizeof(WAVEINCAPS));
	printf("�򿪵������豸��%s\n", wic.szPname);

	// prepare buffer
	m_record=true;
	pBuffer1=(PBYTE)malloc(BUFFER_SIZE);
	pBuffer2=(PBYTE)malloc(BUFFER_SIZE);    //������������ռ�
	
	if (!pBuffer1||!pBuffer2)
	{
		if (pBuffer1) free(pBuffer1);
		if (pBuffer2) free(pBuffer2);
		printf("memory error");
		return 0;
	}

	//��ʼ�������ļ�ͷ
	pWaveHdr1->lpData=(LPSTR)pBuffer1;   //���û�����
	pWaveHdr1->dwBufferLength=BUFFER_SIZE; //��������С
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
	waveInPrepareHeader(hWaveIn,pWaveHdr1,sizeof(WAVEHDR));  //����������Ϣ�������豸����
	waveInAddBuffer(hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ; //����������ַ��ӵ������豸��
	waveInPrepareHeader(hWaveIn,pWaveHdr2,sizeof(WAVEHDR));
	waveInAddBuffer (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ; //
	printf("Start Record!\n\n");

	m_record = true;
	waveInStart(hWaveIn);


	printf ("Press any key to stop recording...\n");
    getch();	
	printf("Record Over!\n\n");
	

	free(pBuffer1);
	free(pBuffer2);

	// clean
	waveInStop(hWaveIn);
	waveInClose(hWaveIn);

	//write file


	FILE * File = fopen("E:\\ecord.pcm","ab+");
	FILE * WAVFile = fopen("E:\\record.wav","ab+");	

	if(File==NULL || WAVFile ==NULL)
	{
		printf("file error");
		return 0;
	}

	//write pcm file
	fwrite(pSaveBuffer,1,dwDataLength, File);
	fclose(File);

	//write wave file

	default_pcmwavhdr.data_size += dwDataLength ; //change file header 
	default_pcmwavhdr.size_8 = default_pcmwavhdr.data_size + 36;
	fwrite(&default_pcmwavhdr,1,sizeof(default_pcmwavhdr), WAVFile); //write wave file
	fwrite(pSaveBuffer,1,dwDataLength, WAVFile);
	fclose(WAVFile);

}

void CALLBACK waveInProc(HWAVEIN hwi,      
						 UINT uMsg,         
						 DWORD_PTR dwInstance,  
						 DWORD_PTR dwParam1,  
						 DWORD_PTR dwParam2     )
{
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;

	if ((WIM_DATA==uMsg) && (buf_count<BUFFER_SIZE))
	{

		int size = pwh->dwBytesRecorded;

		//relocated memory location
		pNewBuffer = (PBYTE)realloc (pSaveBuffer, dwDataLength+ size) ; 

		if(pNewBuffer ==NULL)
		{
			waveInClose(hwi);
			printf("memory error");
			return;
		}

		//copy data into memory and refresh memory address
		pSaveBuffer = pNewBuffer;
		buf_count ++;
		CopyMemory(pSaveBuffer + dwDataLength, pwh->lpData,size);
		dwDataLength += size;

		if(!m_record)
		{
			waveInClose(hwi);
			printf("stop recording");
			return;
		}		
		waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));
	}
}

