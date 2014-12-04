LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../Libs/cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../Libs/cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../Libs/cocos2d/cocos)

LOCAL_MODULE := gameshared
LOCAL_MODULE_FILENAME := libgame

CPP_FILES := $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)  

LOCAL_SRC_FILES := cpp/main.cpp
LOCAL_SRC_FILES += $(CPP_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_STATIC_LIBRARIES := curl_static_prebuilt
LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static

#Synchronize game data folder with project assets folder
LOCAL_DATA_PATH := $(LOCAL_PATH)/../../Resources/
RSYNC_RESULT := $(shell rsync -rtv  $(LOCAL_DATA_PATH) assets/)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)