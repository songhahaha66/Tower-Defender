#pragma once // ��ֹͷ�ļ����ظ�����

#include <graphics.h> // ���� EasyX ͼ�ο�

// ��Ϸ�������
class GameObject
{
public:
    int x, y; // ���������
    int width, height; // ����Ŀ��
    bool active; // �����Ƿ�/���

    GameObject(int x, int y, int w, int h);
    virtual ~GameObject(); // ��������������֤����������ܱ���ȷ����

    // ���麯����Ҫ�����������඼����ʵ���Լ��Ļ��Ʒ���
    virtual void draw() = 0;
    // �麯������������Ը�����Ҫ��д�����߼�
    virtual void update();
};