/* Header includes -----------------------------------------------------------*/
#include "RingBuffer.h"
#include <stdio.h>
#include <windows.h>

/* Macro definitions ---------------------------------------------------------*/
/* Type definitions ----------------------------------------------------------*/
/* Variable declarations -----------------------------------------------------*/
/* Variable definitions ------------------------------------------------------*/
/* Function declarations -----------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None.
 * @return None.
 */
int main(void)
{
	uint8_t data[256] = {0};

	for (int i = 0; i < sizeof(data); i++)
	{
		data[i] = i;
	}

	RingBuffer *fifo = RingBuffer_Malloc(sizeof(data));

	if (fifo != NULL)
	{
		printf("FIFO�����ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
			   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));

		if (RingBuffer_IsFull(fifo) == true)
		{
			printf("FIFO���ˣ�����\n");
		}
		else
		{
			printf("FIFOû��������\n");
		}

		if (RingBuffer_IsEmpty(fifo) == true)
		{
			printf("FIFO���ˣ�����\n");
		}
		else
		{
			printf("FIFOû�գ�����\n");
		}

		printf("\n");

		for (;;)
		{
			{
				if (RingBuffer_In(fifo, data, sizeof(data) / 2) > 0)
				{
					printf("FIFOд��ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}
				else
				{
					printf("FIFOд��ʧ�ܣ�FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n");

			{
				uint8_t rdata[64] = {0};
				uint8_t len = RingBuffer_Out(fifo, rdata, sizeof(rdata));

				if (len > 0)
				{
					printf("��FIFO�ж��������ݣ����ȣ�%d\n", len);

					for (int i = 0; i < len; i++)
					{
						printf("%d ", rdata[i]);
					}

					printf("\n");

					printf("FIFO��ȡ�ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}
				else
				{
					printf("FIFO��ȡʧ�ܣ�FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n");

			{
				RingBuffer_Reset(fifo);
				printf("FIFO��ճɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
					   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n");

			{
				if (RingBuffer_In(fifo, data, sizeof(data)) > 0)
				{
					printf("FIFOд��ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}
				else
				{
					printf("FIFOд��ʧ�ܣ�FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n");

			{
				if (RingBuffer_In(fifo, data, sizeof(data)) > 0)
				{
					printf("FIFOд��ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}
				else
				{
					printf("FIFOд��ʧ�ܣ�FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n");

			{
				uint8_t rdata[256] = {0};
				uint16_t len = RingBuffer_Out(fifo, rdata, sizeof(rdata));

				if (len > 0)
				{
					printf("��FIFO�ж��������ݣ����ȣ�%d\n", len);

					for (int i = 0; i < len; i++)
					{
						printf("%d ", rdata[i]);
					}

					printf("\n");

					printf("FIFO��ȡ�ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}
				else
				{
					printf("FIFO��ȡʧ�ܣ�FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n");

			{
				uint8_t rdata[256] = {0};
				uint16_t len = RingBuffer_Out(fifo, rdata, sizeof(rdata));

				if (len > 0)
				{
					printf("��FIFO�ж��������ݣ����ȣ�%d\n", len);

					for (int i = 0; i < len; i++)
					{
						printf("%d ", rdata[i]);
					}

					printf("\n");

					printf("FIFO��ȡ�ɹ���FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}
				else
				{
					printf("FIFO��ȡʧ�ܣ�FIFO��С��%d��ʹ�ô�С��%d��ʣ���С��%d\n",
						   RingBuffer_Size(fifo), RingBuffer_Len(fifo), RingBuffer_Avail(fifo));
				}

				if (RingBuffer_IsFull(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû��������\n");
				}

				if (RingBuffer_IsEmpty(fifo) == true)
				{
					printf("FIFO���ˣ�����\n");
				}
				else
				{
					printf("FIFOû�գ�����\n");
				}
			}

			printf("\n\n\n");
			Sleep(5000);
		}
	}
	else
	{
		printf("FIFO����ʧ��\n");
	}

	for (;;)
	{
	}
}
