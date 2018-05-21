#pragma once

/// <summary>
/// ����: ��ũ�� ������ ��� ���⽱�� ó���ϱ� ���� �Ѵ�.
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-21
/// </summary>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// ���α׷����� ��� ���� ��ũ�θ� �����մϴ�.
#define	FRAME_BUFFER_WIDTH		1280
#define	FRAME_BUFFER_HEIGHT		720

#define	SWAP_CHAIN_BUFFER_CNT	2
#define	RENDER_TARGET_BUFFER_CNT	5

#define RESOURCE_TEXTURE_2D			0x01
#define RESOURCE_TEXTURE_2D_ARRAY	0x02
#define RESOURCE_TEXTURE_2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

#define MAX_LIGHTS				8 
#define MAX_MATERIALS			1

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

#define TERRAIN_SIZE_WIDTH 10000
#define TERRAIN_SIZE_HEIGHT 5000

#define TERRAIN_IMAGE_WIDTH 500
#define TERRAIN_IMAGE_HEIGHT 250
#define TERRAIN_IMAGE_SCALE XMFLOAT3(TERRAIN_SIZE_WIDTH / TERRAIN_IMAGE_WIDTH, 0.7, TERRAIN_SIZE_HEIGHT / TERRAIN_IMAGE_HEIGHT)

#define REVISE_HEIGHT 153
#define REVISE_HEIGHT_FOR_OBJECT REVISE_HEIGHT + 7.0f

#define NODE_SIZE 41.4

#define NONE -1

// �ִ�, �ּ� ��ġ
#define MAX_MINION 300

// �ִ�, �ּ� �ӵ�
#define MIN_CAMERA_SPEED 100
#define MAX_CAMERA_SPEED 3000

// ���� ����
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

// ���� ��ȯ �Լ�
#define CONVERT_PaperUnit_to_InG(val) val * 8.28
#define CONVERT_Unit_to_InG(val) val * 1000
#define CONVERT_cm_to_InG(val) val * 34.5

// ����ġ ������
#define INCREASE_STATICOBJECT_EXP 5
#define INCREASE_PLAYER_EXP 110
#define INCREASE_MINION_EXP 5

//���� ���� ����----------------
#define Compute_Defence(val) (100)/(100+val)


// �÷��̾� ���⺰ ������-----------
// ü��
#define INCREASE_SWORD_PLAYER_HP 84
#define INCREASE_STAFF_PLAYER_HP 90
#define INCREASE_BOW_PLAYER_HP 50
// ����
#define INCREASE_SWORD_PLAYER_MP 30
#define INCREASE_STAFF_PLAYER_MP 50
#define INCREASE_BOW_PLAYER_MP 60
// ���ݷ�
#define INCREASE_SWORD_PLAYER_ATK 4.5
#define INCREASE_STAFF_PLAYER_ATK 2.41
#define INCREASE_BOW_PLAYER_ATK 2.9
// ����
#define INCREASE_SWORD_PLAYER_DEF 3
#define INCREASE_STAFF_PLAYER_DEF 3.25
#define INCREASE_BOW_PLAYER_DEF 3.4

// �̴Ͼ� ���⺰ �������ͽ� ������
// ü��
#define INCREASE_SWORD_MINION_HP 20
#define INCREASE_STAFF_MINION_HP 10
#define INCREASE_BOW_MINION_HP 15
// ���ݷ�
#define INCREASE_SWORD_MINION_ATK 1
#define INCREASE_STAFF_MINION_ATK 2
#define INCREASE_BOW_MINION_ATK 3
// ����
#define INCREASE_SWORD_MINION_DEF 2
#define INCREASE_STAFF_MINION_DEF 1
#define INCREASE_BOW_MINION_DEF 0

// ���۸��� �������ͽ� ������
// ü��
#define INCREASE_COMMON_CAM_HP 90
#define INCREASE_COMMON_GOLEM_HP 180
// ���ݷ�
#define INCREASE_COMMON_CAM_ATK 4
#define INCREASE_COMMON_GOLEM_ATK 8
// ����
#define INCREASE_COMMON_CAM_DEF 1
#define INCREASE_COMMON_GOLEM_DEF 2

// Ÿ�� ������
// 1�� Ÿ��
#define INCREASE_STATICOBJECT1_ATK 5
#define INCREASE_STATICOBJECT2_ATK 6
// 2�� Ÿ��
#define INCREASE_STATICOBJECT1_DEF 2
#define INCREASE_STATICOBJECT2_DEF 3


// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 1
#define USE_LIMIT_MAX_FPS		 1
#define USE_CONSOLE_WINDOW 0
#define USE_INSTANCING 0
#define USE_BATCH_MATERIAL 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// �ִ� FPS
#if USE_LIMIT_MAX_FPS
#define MAX_FPS 1.0 / 120.0
#else
#define MAX_FPS 0.0
#endif

#define TITLE_MAX_LENGTH 64

#if USE_TITLE_STRING
#define TITLESTRING	TEXT("LegendOfDoodles")
#endif

#if USE_CONSOLE_WINDOW
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#elif _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#endif