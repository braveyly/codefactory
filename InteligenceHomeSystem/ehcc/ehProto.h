#ifndef _EHCC_PROTO__
#define _EHCC_PROTO_

/* ==================================== Indoor phone part(Start) ============================================*/
typedef enum
{
    PPI_DP_DELAYTIME_ENABLE  = 0x00,
    PPI_DP_DELAYTIME_DISABLE = 0x01,
}PROTO_IDP_DP_DELAYTIME_VALUE;

typedef enum
{
    PPI_DP_STATUS_DISABLE    = 0x01,
    PPI_DP_STATUS_ENABLE     = 0x02,
}PROTO_IDP_DP_ACTIVE_STATUS_VALUE;

typedef enum
{
    PPI_DP_ATTRIBUTE_USER_SET    = 0x01,
    PPI_DP_ATTRIBUTE_OPENFOREVER = 0x02,
}PROTO_IDP_DP_ATTRIBUTE_VALUE;

typedef enum
{
    PPI_DP_NORMAL                = 0x01,
    PPI_DP_ALERT                 = 0x02,
}PROTO_IDP_DP_ALERT_STATUS_VALUE;

typedef enum
{
    PPI_CARD_START_REGISTER      =   0x01,
    PPI_CARD_END_REGISTER        =   0x02,
}PROTO_IDP_CARD_REGISTER_FLAG;

typedef enum
{
    PPI_DP_NORMAL_STATUS         =   0x01,
    PPI_DP_ACTIVE_ALARM          =   0x02,
    PPI_DP_INACTIVE_ALARM        =   0x03,
}PROTO_IDP_ALARM_TYPE;

typedef enum
{
    PPI_CAM_TYPE_IP              =   0x01,
    PPI_CAM_TYPE_DVR             =   0x02,
    PPI_CAM_TYPE_NVR             =   0x03,
}PROTO_IDP_CAMERA_TYPE;

typedef enum
{
    PPI_EDIT_CAM_TYPE_NAME      =    0x01,
    PPI_EDIT_CAM_TYPE_IP        =    0x02,
    PPI_EDIT_CAM_TYPE_DELETE    =    0x03,
}PROTO_IDP_CAMERA_EDITTYPE;

typedef enum
{
    PPI_PAIR_SUCCESS    =   0x01,
    PPI_PAIR_FAILED     =   0x02,
}PROTO_IDP_PAIR_RESULT;

typedef enum
{
    PPI_PAIR_REASON_SUCCESS               =  0x00,
    PPI_PAIR_REASON_INVALID_ACCOUNT_INFO  =  0x01,
    PPI_PAIR_REASON_CANNOT_FIND           =  0x02,
}PROTO_IDP_PAIR_FAIL_REASON;

typedef enum
{
    PPI_TOKEN_STATUS_FREE   =   0x01,
    PPI_TOKEN_STATUS_BUSY   =   0x02,
}PROTO_IDP_TOKEN_STATUS;

typedef enum
{
    PPI_OPERATE_TOKEN_REQUEST   =   0x01,
    PPI_OPERATE_TOKEN_RELEASE   =   0x02,
}PROTO_IDP_OPERATE_TOKEN;

/* ==================================== Indoor phone part(End)   ============================================*/


/* ==================================== Outdoor phone part(Start) ============================================*/
typedef enum
{
    PPO_CARD_EDIT       = 0x01,
    PPO_CARD_OPENDOOR   = 0x02,
    PPO_CARD_NOEXIST    = 0x03,
    PPO_CARD_SECURITY   = 0x04,
}PROTO_ODP_REPORT_CARD_ACK_VALUE;

typedef enum
{
    PPO_DOOR_FRONT      = 0x01,
    PPO_DOOR_BACK       = 0x02,
}PROTO_ODP_DOOR_TYPE_VALUE;

typedef enum
{
    PPO_SECURITY_MODE_CLOSE = 0x01,
    PPO_SECURITY_MODE_OPEN  = 0x02,
}PROTO_ODP_SECURITY_MODE_STATUS_VALUE;

typedef enum
{
    PPO_MEDIA_TYPE_VEDIO   = 0x01,
    PPO_MEDIA_TYPE_PICTURE = 0x02,
    PPO_MEDIA_TYPE_NONE    = 0x03,
}PROTO_ODP_MEDIA_TYPE_VALUE;

typedef enum
{
    PPO_OUTDOOR_STATUS_OPEN          =  0x01,  
    PPO_OUTDOOR_STATUS_CLOSED        =  0x02,
}PROTO_ODP_OUTDOOR_STATUS;

typedef enum
{
    PPO_PAIR_SUCCESS    =   0x01,
    PPO_PAIR_FAILED     =   0x02,
}PROTO_ODP_PAIR_RESULT;

typedef enum
{
    PPO_PAIR_REASON_SUCCESS          =  0x00,  
    PPO_PAIR_REASON_INVALID_ACCOUNT  =  0x01,
    PPO_PAIR_REASON_INVALID_PASSWORD =  0x02,
}PROTO_ODP_PAIR_FAIL_REASON;

/* ==================================== Outdoor phone part(End)   ============================================*/

typedef enum
{
    ACT_RESULT_SUCCEED  =   0x01,
    ACT_RESULT_FAILED   =   0x02,
}ACTRESULT;
#endif

