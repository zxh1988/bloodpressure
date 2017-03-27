/*************************************************
  Copyright (C), 2015-2020, ����������ɷ����޹�˾
  @File name:
  @Author:   	xianhui zhang
  @Date:     	2016-09-02
  @Description:
*************************************************/

#ifndef HDR_H
#define HDR_H

typedef enum E_STATUS
{
	UNKNOW_STATUS	= -1,
	SUCCESS,				//�����ɹ�
	FAIL,					//����ʧ��
	NO_RECORD_DATA,			//�����޼�¼����
	EXCEPTION,				//�����豸�쳣
	DISCONNECT,				//�豸ռ�û򲻴���
	TIMEOUT,				//���豸��ʱ
	WRITE_DEVICE_FAIL,		//д�豸ʧ��
	READ_DEVICE_FAIL,		//���豸ʧ��
	JSON_FORMAT_ERROR,		//JSON��ʽ����
	OUT_OF_MEMORY			//�ڴ治��

}E_Status_t;






#endif
