// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "IAudioInterfacesCommonData.h"

// General macros.
//#define ENABLE_AUDIO_PORT_MESSAGES

#if defined(ENABLE_AUDIO_PORT_MESSAGES) && defined(_MSC_VER)
	#define AUDIO_STRINGANIZE2(x) # x
	#define AUDIO_STRINGANIZE1(x) AUDIO_STRINGANIZE2(x)
	#define TODO(y)               __pragma(message(__FILE__ "(" AUDIO_STRINGANIZE1(__LINE__) ") : " "[AUDIO] TODO >>> " AUDIO_STRINGANIZE2(y)))
	#define REINST_FULL(y)        __pragma(message(__FILE__ "(" AUDIO_STRINGANIZE1(__LINE__) ") : " "[AUDIO] REINST " __FUNCSIG__ " >>> " AUDIO_STRINGANIZE2(y)))
	#define REINST(y)             __pragma(message(__FILE__ "(" AUDIO_STRINGANIZE1(__LINE__) ") : " "[AUDIO] REINST " __FUNCTION__ " >>> " AUDIO_STRINGANIZE2(y)))
#else
	#define TODO(y)
	#define REINST_FULL(y)
	#define REINST(y)
#endif

//! \note We need this explicitly here to prevent circular includes to IEntity.
//! Unique identifier for each entity instance.
typedef unsigned int EntityId;
#define INVALID_ENTITYID ((EntityId)(0))

/**
 * @namespace CryAudio
 * @brief Most parent audio namespace used throughout the entire engine.
 */
namespace CryAudio
{
// Forward declarations.
namespace Impl
{
struct IAudioImpl;
}

enum ESystemEvents : EnumFlagsType
{
	eSystemEvent_None            = 0,
	eSystemEvent_ImplSet         = BIT(0),
	eSystemEvent_TriggerExecuted = BIT(1),
	eSystemEvent_TriggerFinished = BIT(2),
	eSystemEvent_FilePlay        = BIT(3),
	eSystemEvent_FileStarted     = BIT(4),
	eSystemEvent_FileStopped     = BIT(5),
	eSystemEvent_All             = 0xFFFFFFFF,
};

enum EDataScope : EnumFlagsType
{
	eDataScope_None,
	eDataScope_Global,
	eDataScope_LevelSpecific,
	eDataScope_All,
};

enum EOcclusionType : EnumFlagsType
{
	eOcclusionType_None,
	eOcclusionType_Ignore,
	eOcclusionType_Adaptive,
	eOcclusionType_Low,
	eOcclusionType_Medium,
	eOcclusionType_High,

	eOcclusionType_Count
};

struct SCreateObjectData
{
	explicit SCreateObjectData(
	  char const* const _szName = nullptr,
	  EOcclusionType const _occlusionType = eOcclusionType_Ignore,
	  CObjectTransformation const& _transformation = s_nullAudioObjectTransformation,
	  EntityId const _entityToIgnore = INVALID_ENTITYID,
	  bool const _bSetCurrentEnvironments = false)
		: szName(_szName)
		, occlusionType(_occlusionType)
		, transformation(_transformation)
		, entityToIgnore(_entityToIgnore)
		, bSetCurrentEnvironments(_bSetCurrentEnvironments)
	{}

	static SCreateObjectData const& GetEmptyObject() { static SCreateObjectData const emptyInstance; return emptyInstance; }

	char const* const               szName;
	EOcclusionType const            occlusionType;
	CObjectTransformation const     transformation; // reference?
	EntityId const                  entityToIgnore;
	bool const                      bSetCurrentEnvironments;
};

struct SExecuteTriggerData : public SCreateObjectData
{
	explicit SExecuteTriggerData(
	  char const* const _szName,
	  EOcclusionType const _occlusionType,
	  CObjectTransformation const& _transformation,
	  bool const _bSetCurrentEnvironments,
	  ControlId const _triggerId,
	  SRequestUserData const& _userData = SRequestUserData::GetEmptyObject())
		: SCreateObjectData(_szName, _occlusionType, _transformation, _bSetCurrentEnvironments)
		, triggerId(_triggerId)
		, userData(_userData)
	{}

	ControlId const        triggerId;
	SRequestUserData const userData;   // reference?
};

/**
 * @struct CryAudio::IObject
 * @brief A struct exposing public methods to interact with audio objects.
 */
struct IObject
{
	// <interfuscator:shuffle>
	/** @cond */
	virtual ~IObject() = default;
	/** @endcond */

	/**
	 * Executes the passed trigger ID on this audio object.
	 * @param triggerId - ID of the trigger to execute.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see StopTrigger
	 */
	virtual void ExecuteTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Stops all instances of the passed trigger ID or all instances of all active triggers if CryAudio::InvalidControlId (default) is passed on this audio object.
	 * @param triggerId - ID of the trigger to stop.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see ExecuteTrigger
	 */
	virtual void StopTrigger(ControlId const triggerId = InvalidControlId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Sets this audio object's transformation.
	 * @param transformation - const reference to the object holding the transformation to apply.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetTransformation(CObjectTransformation const& transformation, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Sets a parameter to a given value on this audio object.
	 * @param parameterId - ID of the parameter in question.
	 * @param value - floating point value to which the parameter should be set.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetParameter(ControlId const parameterId, float const value, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Sets a switch to a given state on this audio object.
	 * @param audioSwitchId - ID of the switch in question.
	 * @param audioSwitchStateId - ID of the switch's state in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetSwitchState(ControlId const audioSwitchId, SwitchStateId const audioSwitchStateId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Sets an environment to a given value on this audio object.
	 * @param audioEnvironmentId - ID of the environment in question.
	 * @param amount - floating point value indicating the amount of the environment to apply.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetEnvironment(EnvironmentId const audioEnvironmentId, float const amount, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Sets all of the environments in which this audio object is located to this audio object.
	 * @param entityToIgnore - ID of the environment providing entity to ignore when determining environments and their values.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetCurrentEnvironments(EntityId const entityToIgnore = INVALID_ENTITYID, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Removes all of the environments currently set to this audio object from this audio object.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void ResetEnvironments(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Sets the occlusion type to be used by this audio object.
	 * @param occlusionType - occlusion type to apply.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetOcclusionType(EOcclusionType const occlusionType, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Plays the given file on this audio object.
	 * @param playFileInfo - reference to a struct that holds data necessary for playback.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see StopFile
	 */
	virtual void PlayFile(SPlayFileInfo const& playFileInfo, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Stops the given file on this audio object.
	 * @param szFile - name of the file in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see PlayFile
	 */
	virtual void StopFile(char const* const szFile, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;
	// </interfuscator:shuffle>
};

/**
 * @struct CryAudio::IListener
 * @brief A struct exposing public methods to interact with audio listeners.
 */
struct IListener
{
	// <interfuscator:shuffle>
	/** @cond */
	virtual ~IListener() = default;
	/** @endcond */

	/**
	 * Sets the listener's transformation.
	 * @param transformation - constant reference to an object holding the transformation to apply.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetTransformation(CObjectTransformation const& transformation, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;
	// </interfuscator:shuffle>
};

struct IAudioSystem
{
	// <interfuscator:shuffle>
	/** @cond */
	virtual ~IAudioSystem() = default;
	/** @endcond */

	/**
	 * This is called during shutdown of the engine which releases AudioSystem resources.
	 * @return void
	 */
	virtual void Release() = 0;

	/**
	 * Used by audio middleware implementations to register themselves with the AudioSystem.
	 * @param pIImpl - pointer to the audio middleware implementation to register.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetImpl(Impl::IAudioImpl* const pIImpl, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Loads a trigger's data. This can be used in "loading on demand" situations to prepare the data referenced by a trigger.
	 * @param triggerId - ID of the trigger in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see UnloadTrigger
	 */
	virtual void LoadTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Unloads a trigger's data. This can be used in "unloading on demand" situations to free the data referenced by a trigger.
	 * @param triggerId - ID of the trigger in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see LoadTrigger
	 */
	virtual void UnloadTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Performs the actions passed in the "triggerData" parameter on an audio object and then executes the passed trigger ID.
	 * For convenience and efficiency this is used as a "fire and forget" type action where the user does not need to explicitly handle an audio object.
	 * @param triggerData - reference to an object that holds all of the data necessary for the trigger execution.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void ExecuteTriggerEx(SExecuteTriggerData const& triggerData, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Executes the passed trigger ID. This is used for "fire and forget" type operations and for 2D type events exclusively.
	 * @param triggerId - ID of the trigger to execute.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see StopTrigger
	 */
	virtual void ExecuteTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Stops all instances of the passed trigger ID or all instances of all active triggers if CryAudio::InvalidControlId (default) is passed.
	 * @param triggerId - ID of the trigger to stop.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see ExecuteTrigger
	 */
	virtual void StopTrigger(ControlId const triggerId = InvalidControlId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Globally set a parameter to a given value.
	 * @param parameterId - ID of the parameter in question.
	 * @param value - floating point value to which the parameter should be set.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetParameter(ControlId const parameterId, float const value, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Globally set a switch to a given state.
	 * @param audioSwitchId - ID of the switch in question.
	 * @param audioSwitchStateId - ID of the switch's state in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void SetSwitchState(ControlId const audioSwitchId, SwitchStateId const audioSwitchStateId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Globally plays a file.
	 * @param playFileInfo - reference to a struct that holds data necessary for playback.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see StopFile
	 */
	virtual void PlayFile(SPlayFileInfo const& playFileInfo, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Globally stops a file.
	 * @param szFile - name of the file in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see PlayFile
	 */
	virtual void StopFile(char const* const szFile, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used by audio middleware implementations to inform the AudioSystem that a file started playback.
	 * @param standaloneFile - reference to the instance of the file that started playback.
	 * @param bSuccessfulyStarted - boolean indicating whether playback started successfully or not.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see ReportStoppedFile
	 */
	virtual void ReportStartedFile(CATLStandaloneFile& standaloneFile, bool bSuccessfulyStarted, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used by audio middleware implementations to inform the AudioSystem that a file stopped playback.
	 * @param standaloneFile - reference to the instance of the file that stopped playback.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see ReportStartedFile
	 */
	virtual void ReportStoppedFile(CATLStandaloneFile& standaloneFile, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used by audio middleware implementations to inform the AudioSystem that an event finished producing sound.
	 * @param standaloneFile - reference to the instance of the file that finished producing sound.
	 * @param bSuccess - boolean indicating whether the event finished successfully or not.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void ReportFinishedEvent(CATLEvent& audioEvent, bool const bSuccess, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used by the engine to inform the AudioSystem that the application window lost focus.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see GotFocus
	 */
	virtual void LostFocus(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used by the engine to inform the AudioSystem that the application window got focus.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see LostFocus
	 */
	virtual void GotFocus(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used to instruct the AudioSystem that it should mute all active sounds.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see UnmuteAll
	 */
	virtual void MuteAll(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used to instruct the AudioSystem that it should un-mute all active sounds.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see MuteAll
	 */
	virtual void UnmuteAll(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used to instruct the AudioSystem that it should stop all playing sounds.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void StopAllSounds(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used instruct the AudioSystem that it should reload the registered audio middleware.
	 * This is useful when for instance an audio project changed while the application was running.
	 * @param szLevelName - name of the currently loaded level so that level specific data gets reloaded as well.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void RefreshAudioSystem(char const* const szLevelName, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Loads all of the data referenced by the given preload request.
	 * @param audioPreloadRequestId - ID of the preload request in question.
	 * @param bAutoLoadOnly - boolean indicating whether to load the given preload request only if it's been set to AutoLoad.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see UnloadSingleRequest
	 */
	virtual void PreloadSingleRequest(PreloadRequestId const audioPreloadRequestId, bool const bAutoLoadOnly, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Unloads all of the data referenced by the given preload request.
	 * @param audioPreloadRequestId - ID of the preload request in question.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 * @see PreloadSingleRequest
	 */
	virtual void UnloadSingleRequest(PreloadRequestId const audioPreloadRequestId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Reloads all of the audio controls and their connections.
	 * @param szFolderPath - path to where the audio controls data has been stored.
	 * @param szLevelName - name of the currently loaded level to also reload level specific data.
	 * @param userData - optional struct used to pass additional data to the internal request.
	 * @return void
	 */
	virtual void ReloadControlsData(char const* const szFolderPath, char const* const szLevelName, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) = 0;

	/**
	 * Used to register a callback function that is called whenever a given event occurred.
	 * @param func - address of the function to be called.
	 * @param pObjectToListenTo - address of the object in which events one is interested. If set to nullptr events of the given type produced by any object will be listened to.
	 * @param eventMask - a combination of CryAudio::ESystemEvents one is interested in. If set to eAudioSystemEvent_All, all events generated by the given object will be received.
	 * @return void
	 * @see RemoveRequestListener
	 */
	virtual void AddRequestListener(void (* func)(SRequestInfo const* const), void* const pObjectToListenTo, EnumFlagsType const eventMask) = 0;

	/**
	 * Used to unregister a callback function.
	 * @param func - address of the function to be called.
	 * @param pObjectToListenTo - address of the object in which events one is interested. If set to nullptr events of the given type produced by any object will be listened to.
	 * @param eventMask - a combination of CryAudio::ESystemEvents one is interested in. If set to eAudioSystemEvent_All, all events generated by the given object will be received.
	 * @return void
	 * @see AddRequestListener
	 */
	virtual void RemoveRequestListener(void (* func)(SRequestInfo const* const), void* const pObjectToListenTo) = 0;

	/**
	 * Ideally called by the application's main thread.
	 * @return void
	 */
	virtual void ExternalUpdate() = 0;

	/**
	 * Used to retrieve an audio trigger's ID.
	 * @param szAudioTriggerName - name of the audio trigger which ID is to be retrieved.
	 * @param audioTriggerId - out parameter that receives the ID.
	 * @return bool - returns true if the control was found and the ID retrieved successfully otherwise false.
	 */
	virtual bool GetAudioTriggerId(char const* const szAudioTriggerName, ControlId& audioTriggerId) const = 0;

	/**
	 * Used to retrieve an audio parameter's ID.
	 * @param szParameterName - name of the audio parameter which ID is to be retrieved.
	 * @param parameterId - out parameter that receives the ID.
	 * @return bool - returns true if the control was found and the ID retrieved successfully otherwise false.
	 */
	virtual bool GetAudioParameterId(char const* const szParameterName, ControlId& parameterId) const = 0;

	/**
	 * Used to retrieve an audio switch's ID.
	 * @param szAudioSwitchName - name of the audio switch which ID is to be retrieved.
	 * @param audioSwitchId - out parameter that receives the ID.
	 * @return bool - returns true if the control was found and the ID retrieved successfully otherwise false.
	 */
	virtual bool GetAudioSwitchId(char const* const szAudioSwitchName, ControlId& audioSwitchId) const = 0;

	/**
	 * Used to retrieve an audio switch's state ID.
	 * @param audioSwitchId - ID of the switch to which the state belongs.
	 * @param szSwitchStateName - name of the audio switch state which ID is to be retrieved.
	 * @param audioSwitchStateId - out parameter that receives the ID.
	 * @return bool - returns true if the control was found and the ID retrieved successfully otherwise false.
	 */
	virtual bool GetAudioSwitchStateId(ControlId const audioSwitchId, char const* const szSwitchStateName, SwitchStateId& audioSwitchStateId) const = 0;

	/**
	 * Used to retrieve an audio preload request's ID.
	 * @param szAudioPreloadRequestName - name of the audio preload request which ID is to be retrieved.
	 * @param audioPreloadRequestId - out parameter that receives the ID.
	 * @return bool - returns true if the control was found and the ID retrieved successfully otherwise false.
	 */
	virtual bool GetAudioPreloadRequestId(char const* const szAudioPreloadRequestName, PreloadRequestId& audioPreloadRequestId) const = 0;

	/**
	 * Used to retrieve an audio environment's ID.
	 * @param szAudioEnvironmentName - name of the audio environment which ID is to be retrieved.
	 * @param audioPreloadRequestId - out parameter that receives the ID.
	 * @return bool - returns true if the control was found and the ID retrieved successfully otherwise false.
	 */
	virtual bool GetAudioEnvironmentId(char const* const szAudioEnvironmentName, EnvironmentId& audioEnvironmentId) const = 0;

	/**
	 * Returns the path in which audio data is stored.
	 * @return char const pointer to the string holding the location.
	 */
	virtual char const* GetConfigPath() const = 0;

	/**
	 * Constructs an instance of an audio listener.
	 * Note: Retrieving a listener this way requires the instance to be freed via ReleaseListener once not needed anymore!
	 * @return Pointer to a freshly constructed CryAudio::IListener instance.
	 * @see ReleaseListener
	 */
	virtual IListener* CreateListener() = 0;

	/**
	 * Destructs the passed audio listener instance.
	 * @param pIListener - Pointer to the audio listener that needs destruction.
	 * @return void
	 * @see CreateListener
	 */
	virtual void ReleaseListener(IListener* const pIListener) = 0;

	/**
	 * Constructs an instance of an audio object.
	 * Note: Retrieving an object this way requires the object to be freed via ReleaseObject once not needed anymore!
	 * @param objectData - optional data used during audio object construction.
	 * @return Pointer to a freshly constructed CryAudio::IObject instance.
	 * @see ReleaseObject
	 */
	virtual IObject* CreateObject(SCreateObjectData const& objectData = SCreateObjectData::GetEmptyObject()) = 0;

	/**
	 * Destructs the passed audio object instance.
	 * @param pIObject - Pointer to the audio object that needs destruction.
	 * @return void
	 * @see CreateObject
	 */
	virtual void ReleaseObject(IObject* const pIObject) = 0;

	/**
	 * Retrieve an audio file's attributes.
	 * @param szFilename - name of the file in question.
	 * @param audioFileData - out parameter which receives the file's data.
	 * @return void
	 */
	virtual void GetAudioFileData(char const* const szFilename, SFileData& audioFileData) = 0;

	/**
	 * Retrieve an audio trigger's attributes.
	 * @param audioTriggerId - id of the trigger in question.
	 * @param audioTriggerData - out parameter which receives the trigger's data.
	 * @return void
	 */
	virtual void GetAudioTriggerData(ControlId const audioTriggerId, STriggerData& audioTriggerData) = 0;

	/**
	 * This method is called by the LevelSystem whenever a level is loaded.
	 * It allows the AudioSystem to handle its data accordingly.
	 * @param szLevelName - name of the level that is being loaded.
	 * @return void
	 */
	virtual void OnLoadLevel(char const* const szLevelName) = 0;

	/**
	 * This method is called by the LevelSystem whenever a level is unloaded.
	 * It allows the AudioSystem to handle its data accordingly.
	 * @return void
	 */
	virtual void OnUnloadLevel() = 0;

	/**
	 * This method is called whenever the language is changed.
	 * It allows the AudioSystem to reload language specific data.
	 * @return void
	 */
	virtual void OnLanguageChanged() = 0;
	// </interfuscator:shuffle>
};
} // namespace CryAudio

AUTO_TYPE_INFO(CryAudio::EOcclusionType);
