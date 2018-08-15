#pragma once

/// <summary>
/// ����: ��ũ�� ������ ��� ���⽱�� ó���ϱ� ���� �Ѵ�.
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-04
/// </summary>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// ���α׷����� ��� ���� ��ũ�θ� �����մϴ�.
#define	FRAME_BUFFER_WIDTH		1280
#define	FRAME_BUFFER_HEIGHT		720

#define	SWAP_CHAIN_BUFFER_CNT		2
#define	RENDER_TARGET_BUFFER_CNT	8

#define RESOURCE_TEXTURE_2D			0x01
#define RESOURCE_TEXTURE_2D_ARRAY	0x02
#define RESOURCE_TEXTURE_2DARRAY	0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

#define MAX_LIGHTS				8 
#define MAX_MATERIALS			1

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

#define SHADOW_MAP_SIZE 4096

// ����(Terrain) ����
#define TERRAIN_SIZE_WIDTH 10000.0f
#define TERRAIN_SIZE_HEIGHT 5000.0f
#define TERRAIN_SIZE_DIAGONAL sqrt((TERRAIN_SIZE_WIDTH * TERRAIN_SIZE_WIDTH) + (TERRAIN_SIZE_HEIGHT * TERRAIN_SIZE_HEIGHT))
#define TERRAIN_SIZE_DIAGONAL_ADJ static_cast<int>(sqrt((TERRAIN_SIZE_WIDTH * TERRAIN_SIZE_WIDTH) + (TERRAIN_SIZE_HEIGHT * TERRAIN_SIZE_HEIGHT)) + 100)

#define TERRAIN_IMAGE_WIDTH 1000.0f
#define TERRAIN_IMAGE_HEIGHT 500.0f
#define TERRAIN_IMAGE_SCALE XMFLOAT3(TERRAIN_SIZE_WIDTH / TERRAIN_IMAGE_WIDTH, 1.4f, TERRAIN_SIZE_HEIGHT / TERRAIN_IMAGE_HEIGHT)

#define TERRAIN_IMAGE_CELL_WIDTH 50.0f
#define TERRAIN_IMAGE_CELL_HEIGHT 25.0f
#define TERRAIN_IMAGE_CELL_SCALE XMFLOAT3(TERRAIN_SIZE_WIDTH / TERRAIN_IMAGE_CELL_WIDTH, 1.4f, TERRAIN_SIZE_HEIGHT / TERRAIN_IMAGE_CELL_HEIGHT)

#define TETRRAIN_COLLISION_WIDTH 1000.0f
#define TETRRAIN_COLLISION_HEIGHT 500.0f
#define TERRAIN_COLLISION_SCALE XMFLOAT3(TERRAIN_SIZE_WIDTH / TETRRAIN_COLLISION_WIDTH, 1.4f, TERRAIN_SIZE_HEIGHT / TETRRAIN_COLLISION_HEIGHT)

// �ð�(Time) ����
#define TIME_AVAILABILITY_CHECK 1.5f	// subPath�� ��ȿ�� �ð�
#define TIME_ACTIVATE_CHECK 3.0f	// �߸� ���Ͱ� ��� ���·� ���ư��� �ð�
#define TIME_ACTIVATE_CHECK_GOLEM 10.0f	// �߸� ���Ͱ� ��� ���·� ���ư��� �ð�

#define COOLTIME_HP_SYNC 0.5f

// ����(Height) ����
#define REVISE_HEIGHT 100.0f
#define REVISE_HEIGHT_FOR_OBJECT REVISE_HEIGHT + 7.0f

// ���(Node) ����
#define NODE_SIZE 41.4f
#define NODE_SIZE_HALF NODE_SIZE * 0.5f
#define NODE_SIZE_SQR NODE_SIZE * NODE_SIZE
#define NODE_SIZE_HALF_SQR NODE_SIZE_HALF * NODE_SIZE_HALF
#define NONE -1

// �ִ�, �ּ� ��ġ
#define MAX_MINION 300
#define MAX_ARROW 70
#define MAX_MAGIC 70
#define MAX_HIT 100
#define MAX_EACH_TOWER_ATK 6	// �� ���� Ÿ�� 6��
#define MAX_PLAYER_ARROW_ATK 30
#define MAX_PLAYER_MAGIC_ATK 30
#define MAX_PLAYER_SKILL 4

// �ִ� �ּ�
#define MAX_QSKILL 4

// Animation Speed
#define ANIMATION_SPEED 30

// �ִ�, �ּ� �ӵ�
#define MIN_CAMERA_SPEED 100.0f
#define MAX_CAMERA_SPEED 3000.0f

// ���� ����
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

// ���� ��ȯ �Լ�
#define CONVERT_PaperUnit_to_InG(val) val * 8.28f
#define CONVERT_Unit_to_InG(val) val * 1000.0f
#define CONVERT_cm_to_InG(val) val * 34.5f

// �߸� ���� ���� �Ÿ�
#define MAX_RANGE_FROM_SPAWN_ROIDER CONVERT_Unit_to_InG(0.489f)
#define MAX_RANGE_FROM_SPAWN_GOLEM CONVERT_Unit_to_InG(0.636f)

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

// Minimap Window Screen ����
#define MINIMAP_MINIMUM_X FRAME_BUFFER_WIDTH / 1.3034f
#define MINIMAP_MAXIMUM_X FRAME_BUFFER_WIDTH / 1.0078f
#define MINIMAP_MINIMUM_Y FRAME_BUFFER_HEIGHT / 1.3333f
#define MINIMAP_MAXIMUM_Y FRAME_BUFFER_HEIGHT / 1.0526f

// W - E ���̴� 104.4 , 94.4
// E - Q ���̴� 104.4 , 114.4
// ������ ��ǥ�� ����
// Skill Window Screen ����
#define QSKILL_MINIMUM_X FRAME_BUFFER_WIDTH  / 2.8495f	// 449.2f
#define QSKILL_MAXIMUM_X FRAME_BUFFER_WIDTH  / 2.5039f	// 625.6f

#define WSKILL_MINIMUM_X FRAME_BUFFER_WIDTH  / 2.3121f	// 553.6f
#define WSKILL_MAXIMUM_X FRAME_BUFFER_WIDTH  / 2.0460f	// 625.6f

#define ESKILL_MINIMUM_X FRAME_BUFFER_WIDTH / 1.9452f	// 658.f
#define ESKILL_MAXIMUM_X FRAME_BUFFER_WIDTH / 1.7777f	// 720.f

#define RSKILL_MINIMUM_X FRAME_BUFFER_WIDTH / 1.6789f	// 762.4f
#define RSKILL_MAXIMUM_X FRAME_BUFFER_WIDTH / 1.5340f	// 834.4f

#define SKILL_MINIMUM_Y FRAME_BUFFER_HEIGHT / 1.2121f	// 594.f
#define SKILL_MAXIMUM_Y FRAME_BUFFER_HEIGHT / 1.0810f	// 666.f

// Special Frame Window Screen ����

#define SPECIAL_MINIMUM_X FRAME_BUFFER_WIDTH / 27.82f	// 46.f
#define SPECIAL_MAXIMUM_X FRAME_BUFFER_WIDTH / 8.311f	// 154.f

#define SPECIAL_MINIMUM_Y FRAME_BUFFER_HEIGHT / 1.25f	// 576.f
#define SPECIAL_MAXIMUM_Y FRAME_BUFFER_HEIGHT / 1.052f	// 684.f

// Attack Special Selecte Window Screen ����
#define SELECT_ATTACK_SPECIAL_MINIMUM_Y FRAME_BUFFER_HEIGHT / 1.998f // 360.36f
#define SELECT_ATTACK_SPECIAL_MAXIMUM_Y FRAME_BUFFER_HEIGHT / 1.713f // 420.12f

// Defence Special Selecte Window Screen ����
#define SELECT_DEFENCE_SPECIAL_MINIMUM_Y FRAME_BUFFER_HEIGHT / 1.713f // 420.12f
#define SELECT_DEFENCE_SPECIAL_MAXIMUM_Y FRAME_BUFFER_HEIGHT / 1.500f // 479.88f

// Technic Special Selecte Window Screen ����
#define SELECT_TECHNIC_SPECIAL_MINIMUM_Y FRAME_BUFFER_HEIGHT / 1.500f // 479.88f
#define SELECT_TECHNIC_SPECIAL_MAXIMUM_Y FRAME_BUFFER_HEIGHT / 1.334f // 539.64f

// ���� x ����
#define SELECT_SPECIAL_MINIMUM_X FRAME_BUFFER_WIDTH / 30.18f	// 42.4f
#define SELECT_SPECIAL_MAXIMUM_X FRAME_BUFFER_WIDTH / 6.86f		// 186.4f


// FoW 243 122 41.4
#define NODE_WIDTH 243
#define NODE_HEIGHT 122
#define NODE_LENGTH NODE_WIDTH*NODE_HEIGHT

// ��Ÿ�� ����
// �÷��̾� ����
#define COOLTIME_SPAWN_PLAYER 10.0f

#define COOLTIME_SKILL_Q 5.0f
#define COOLTIME_SKILL_W 8.0f
#define COOLTIME_SKILL_E 12.0f
#define COOLTIME_SKILL_R 15.0f

// ���� ����
#define COOLTIME_SPAWN_ROIDER 10.0f
#define COOLTIME_SPAWN_GOLEM 10.0f
#define COOLTIME_SPECIAL_ATTACK1 16.0f
#define COOLTIME_SPECIAL_ATTACK2 24.0f

// Ÿ�� ����
#define COOLTIME_TOWER_ATTACK 5.0f

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 1
#define USE_LIMIT_MAX_FPS		 1
#define USE_CONSOLE_WINDOW		 1
#define USE_BATCH_MATERIAL		 1
#define USE_DEBUG_CONTROLLER	 1

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