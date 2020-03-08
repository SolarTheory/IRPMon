
/**
 * @file
 *
 * Exports an interface to the irpmondll.dll library. The library forms an user
 * mode part of the system for monitoring various driver and device-related events.
 *
 *  HOW TO USE
 *
 *  *  Initialize the library by a call to the @link(IRPMonDllInitialize) routine.
 *     The current process is connected to the IRPMon driver and the library is now 
 *     ready to work.
 *  *  Retrieve information about drivers and devices currently present in the system
 *     (@link(IRPMonDllSnapshotRetrieve)). The retrieved data must be released by the
 *     @link(IRPMonDllSnapshotFree) procedure when no longer needed.
 *  *  Retrieve the list of drivers and devices currently monitored (and hooked) by the
 *     IRPMon driver. The @link(IRPMonDllDriverHooksEnumerate) does this. Free the returned
 *     information by using the @link(IRPMonDllDriverHooksFree) procedure. The "HookHandle"
 *     member of the structures describing the hooked objects can be used in library functions
 *     described below.
 *  *  Hook a new driver by specifying its name to the @link(IRPMonDllHookDriver) function. This
 *     call instructs the IRPMon driver to store information about the target driver within its
 *     data structures. The actual monitoring must be started by the @link(IRPMonDllDriverStartMonitoring) 
 *     function. @link(IRPMonDllDriverStopMonitoring) stops the monitoring. 
 *  *  Unhook a given driver by passing its hook handle to the @link(IRPMonDllUnhookDriver) function.
 *     The monitoring must not be active. Otherwise, the function fails. 
 *  *  Use the @link(IRPMonDllDriverSetInfo) to change monitoring settings of a hooked driver. If the monitoring
 *     is active, only the value of the MonitorNewDevices setting is propagated to the IRPMon driver.
 *  *  Determine which device objects of a driver should be monitored. Use @link(IRPMonDllHookDeviceByName),
 *     @link(IRPMonDllHookDeviceByAddress) and @link(IRPMonDllUnhookDevice) to tell this to the IRPMon driver.
 * 
 *   COLLECTIONG EVENTS
 *
 *   * Use the @link(IRPMonDllConnect) to connect the current process to the IRPMon Event Queue. You can supply
 *     a semaphore object the counter of which is increased by the IRPMon driver every time an event is added to
 *     the queue. During initialization of the connection, the driver increments the counter by a number of events
 *     currently stored in the queue. 
 *     At most one process can be connected to the queue at any moment of time.
 *   * Disconnect the process by calling the @link(IRPMonDllDisconnect) function.
 *   * Retrieve individual events from the queue via the @link(IRPMonDllGetRequest)
 *     function.
 */

#ifndef __IRPMONDLL_H__
#define __IRPMONDLL_H__

#include <windows.h>
#include "irpmondll-types.h"




#ifdef IRPMONDLL_EXPORTS

#define IRPMONDLL_API                  EXTERN_C __declspec(dllexport)

#else 

#define IRPMONDLL_API                  EXTERN_C __declspec(dllimport)

#endif



IRPMONDLL_API DWORD WINAPI IRPMonDllDriverHooksEnumerate(PHOOKED_DRIVER_UMINFO *HookedDrivers, PULONG Count);
IRPMONDLL_API VOID WINAPI IRPMonDllDriverHooksFree(PHOOKED_DRIVER_UMINFO HookedDrivers, ULONG Count);
IRPMONDLL_API DWORD WINAPI IRPMonDllHookDriver(PWCHAR DriverName, PDRIVER_MONITOR_SETTINGS MonitorSettings, BOOLEAN DeviceExtensionHook, PHANDLE DriverHandle, PVOID *ObjectId);
IRPMONDLL_API DWORD WINAPI IRPMonDllDriverStartMonitoring(HANDLE Driverhandle);
IRPMONDLL_API DWORD WINAPI IRPMonDllDriverStopMonitoring(HANDLE Driverhandle);
IRPMONDLL_API DWORD WINAPI IRPMonDllDriverSetInfo(HANDLE DriverHandle, PDRIVER_MONITOR_SETTINGS Settings);
IRPMONDLL_API DWORD WINAPI IRPMonDllUnhookDriver(HANDLE DriverHandle);
IRPMONDLL_API DWORD WINAPI IRPMonDllHookDeviceByName(PWCHAR DeviceName, PHANDLE HookHandle, PVOID *ObjectId);
IRPMONDLL_API DWORD WINAPI IRPMonDllHookDeviceByAddress(PVOID DeviceObject, PHANDLE HookHandle, PVOID *ObjectId);


/** Stops monitoring events related to a given device object.
 *
 *  @param HookHandle A handle representing the device object being monitored.
 *
 *  @return
 *  The routine can return one of the following values:
 *  @value ERROR_SUCCESS The operation has succeeded. The devices is not being
 *  monitored any more. The handle given in the argument is not valid any longer.
 *  @value ERROR_INVALID_HANDLE The given handle is invalid.
 *
 *  @remark
 *  If the whole driver to which the device belongs is unhooked (see @link(IRPMonDllUnhookDriver)), this
 *  routine needs not to be called since the device handle is automatically invalidated.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllUnhookDevice(HANDLE HookHandle);


/** Retrieves information about driver and device objects currently present
 *  in the system.
 *
 *  @param DriverInfo Address of variable that receives address of an array of
 *  pointers to IRPMON_DRIVER_INFO structures, each containing information about
 *  one driver and its devices.
 *  @param Count Address of variable that receives the number of structures in the 
 *  array.
 *
 *  @return
 *  The routine may return one of the following values:
 *  @value ERROR_SUCCESS The snapshot has been retrieved successfully.
 *  @value Other An error occurred.
 *
 *  @remark
 *  When the caller no longer needs the information retrieved by the routine,
 *  it must free it by calling the @link(IRPMonDllSnapshotFree) procedure.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllSnapshotRetrieve(PIRPMON_DRIVER_INFO **DriverInfo, PULONG Count);


/** Frees a given snapshot of drivers and their devices.
 *
 *  @param DriverInfo Array of pointers to IRPMON_DRIVER_INFO structures, each
 *  containing information about one driver and its devices. Address of the array
 *  is returned in the first parameter of a call to the @link(IRPMonDllSnapshotRetrieve)
 *  function.
 *  @param Count Number of entries in the array. This value is returned in the second parameter
 *  of the @link(IRPMonDllSnapshotRetrieve) call.
 */
IRPMONDLL_API VOID WINAPI IRPMonDllSnapshotFree(PIRPMON_DRIVER_INFO *DriverInfo, ULONG Count);


/** Connects the current thread (the calling one) to the queue
 *  of events detected by the IRPMon driver.
 *
 *  @return
 *  The function returns one of the following error codes:
 *  @value ERROR_SUCCESS The thread successfully connected to the queue.
 *  @value Other An error occurred.
 *
 *  @remark
 *  At most one thread can be connected to the IRPMon Event Queue at any
 *  moment of time.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllConnect(void);


/** Disconnects the current thread from the IRPMon Event Queue.
 *
 *  @return
 *  ne of the following error codes may be returned:
 *  @value ERROR_SUCCESS The thread successfully disconnected from the queue.
 *  @value Other An error occurred.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllDisconnect(VOID);


IRPMONDLL_API DWORD WINAPI IRPMonDllQueueClear(void);

/** Removes a request from the IRPMon Event Queue and copies it to a given
 *  buffer.
 *
 *  @param Request Address of buffer to which the request data will be copied.
 *  @param Size Size of the buffer, in bytes.
 *
 *  @return
 *  Returns one of the following values:
 *  @value ERROR_SUCCESS The request was successfully removed from the queue and
 *  copied to the given buffer. 
 *  @value ERROR_INSUFFICIENT_BUFFER The given buffer is not large enough to 
 *  hold all the request data. Because all types of requests are of fixed size,
 *  hence using a buffer large enough to hold the largest request type will avoid
 *  this error completely.
 *
 *  @remark
 *  The calling thread must be connected to the IRPMon Event Queue. Otherwise,
 *  the function fails.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllGetRequest(PREQUEST_HEADER Request, DWORD Size);


/** Get size of a request, in bytes.
 *
 *  @param Request Pointer to the request retrieved via @link(IRPMonDllGetRequest).
 *
 *  @returns
 *  If successful, returns the request size, in bytes.
 *  On error, zero is returned.
 *
 *  @remark
 *  The routine just looks at request type and performs necessary
 *  calculcations to obtain the total size, including general data
 *  associated with the request.
 */
IRPMONDLL_API size_t WINAPI IRPMonDllGetRequestSize(const REQUEST_HEADER *Request);

/** Open a handle to a given driver monitored by the IRPMon driver.
 *
 *  @param ObjectId ID of the target driver. IDs can be obtained from the
 *  'ObjectId' member of the @link(HOOKED_DRIVER_UMINFO) structure retrieved
 *  by the @link(IRPMonDllDriverHooksEnumerate) function.
 *  @param Handle Address of variable that receives the newly opened handle. The
 *  handle can be then used to control the hooked driver.
 *
 *  @return
 *  The function may return one of the following values:
 *    @value ERROR-SUCCESS The handle has been created successfully.
 *    @value Other An error occurred.
 *
 *  @remark
 *  When no longer needed, the handle should be closed via the @link(IRPMonDllCloseHookedDriverHandle)
 *  function. Alternatively, the @link(IRPMonDllUnhookDriver) routine may also be used to close the
 *  handle, however, it also unhooks the driver represented by the handle.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllOpenHookedDriver(PVOID ObjectId, PHANDLE Handle);


/** Closes a handle to a given driver monitored by the IRPMon.
 *
 *  @param Handle The handle to close.
 *
 *  @return
 *  The routine should always return ERROR-SUCCESS. If it does not, the bug is
 9  in caller's code, not in the code of the library or driver.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllCloseHookedDriverHandle(HANDLE Handle);


/** Open a handle to a given device monitored by the IRPMon driver.
 *
 *  @param ObjectId ID of the target device. IDs can be obtained from the
 *  'ObjectId' member of the @link(HOOKED_DEVICE_UMINFO) structure retrieved
 *  by the @link(IRPMonDllDriverHooksEnumerate) function.
 *  @param Handle Address of variable that receives the newly opened handle. The
 *  handle can be then used to control the hooked device.
 *
 *  @return
 *  The function may return one of the following values:
 *    @value ERROR-SUCCESS The handle has been created successfully.
 *    @value Other An error occurred.
 *
 *  @remark
 *  When no longer needed, the handle should be closed via the @link(IRPMonDllCloseHookedDeviceHandle)
 *  function. Alternatively, the @link(IRPMonDllUnhookDevice) routine may also be used to close the
 *  handle, however, it also unhooks the device represented by the handle.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllOpenHookedDevice(PVOID ObjectId, PHANDLE Handle);


/** Closes a handle to a given device monitored by the IRPMon.
 *
 *  @param Handle The handle to close.
 *
 *  @return
 *  The routine should always return ERROR-SUCCESS. If it does not, the bug is
 9  in caller's code, not in the code of the library or driver.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllCloseHookedDeviceHandle(HANDLE Handle);


IRPMONDLL_API DWORD WINAPI IRPMonDllHookedDeviceGetInfo(HANDLE Handle, PUCHAR IRPSettings, PUCHAR FastIOSettings, PBOOLEAN MonitoringEnabled);
IRPMONDLL_API DWORD WINAPI IRPMonDllHookedDeviceSetInfo(HANDLE Handle, PUCHAR IRPSettings, PUCHAR FastIOSettings, BOOLEAN MonitoringEnabled);
IRPMONDLL_API DWORD WINAPI IRPMonDllHookedDriverGetInfo(HANDLE Handle, PDRIVER_MONITOR_SETTINGS Settings, PBOOLEAN MonitoringEnabled);

IRPMONDLL_API DWORD WINAPI IRPMonDllClassWatchRegister(PWCHAR ClassGuid, BOOLEAN UpperFilter, BOOLEAN Beginning);
IRPMONDLL_API DWORD WINAPI IRPMonDllClassWatchUnregister(PWCHAR ClassGuid, BOOLEAN UpperFilter, BOOLEAN Beginning);
IRPMONDLL_API DWORD WINAPI IRPMonDllClassWatchEnum(PCLASS_WATCH_RECORD *Array, PULONG Count);
IRPMONDLL_API VOID WINAPI IRPMonDllClassWatchEnumFree(PCLASS_WATCH_RECORD Array, ULONG Count);

IRPMONDLL_API DWORD WINAPI IRPMonDllDriverNameWatchRegister(PWCHAR DriverName, PDRIVER_MONITOR_SETTINGS MonitorSettings);
IRPMONDLL_API DWORD WINAPI IRPMonDllDriverNameWatchUnregister(PWCHAR DriverName);
IRPMONDLL_API DWORD WINAPI IRPMonDllDriverNameWatchEnum(PDRIVER_NAME_WATCH_RECORD *Array, PULONG Count);
IRPMONDLL_API VOID WINAPI IRPMonDllDriverNameWatchEnumFree(PDRIVER_NAME_WATCH_RECORD Array, ULONG Count);

IRPMONDLL_API DWORD WINAPI IRPMonDllRequestEmulateDriverDetected(void *DriverObject, const wchar_t *DriverName, PREQUEST_DRIVER_DETECTED *Request);
IRPMONDLL_API DWORD WINAPI IRPMonDllRequestEmulateDeviceDetected(void *DriverObject, void *DeviceObject, const wchar_t *DeviceName, PREQUEST_DEVICE_DETECTED *Request);
IRPMONDLL_API DWORD WINAPI IRPMonDllRequestEmulateFileNameAssigned(void *FileObject, const wchar_t *FileName, PREQUEST_FILE_OBJECT_NAME_ASSIGNED *Request);
IRPMONDLL_API DWORD WINAPI IRPMonDllRequestEmulateFileNameDeleted(void *FileObject, PREQUEST_FILE_OBJECT_NAME_DELETED *Request);
IRPMONDLL_API DWORD WINAPI IRPMonDllRequestEmulateProcessCreated(HANDLE ProcessId, HANDLE ParentId, const wchar_t *ImageName, const wchar_t *CommandLine, PREQUEST_PROCESS_CREATED *Request);
IRPMONDLL_API DWORD WINAPI IRPMonDllRequestEmulateProcessExitted(HANDLE ProcessId, PREQUEST_PROCESS_EXITTED *Request);
IRPMONDLL_API PREQUEST_HEADER WINAPI IRPMonDllRequestCopy(const REQUEST_HEADER *Header);
IRPMONDLL_API PREQUEST_HEADER WINAPI IRPMonDllRequestMemoryAlloc(size_t Size);
IRPMONDLL_API void WINAPI IRPMonDllRequestMemoryFree(PREQUEST_HEADER Header);
IRPMONDLL_API BOOLEAN WINAPI IRPMonDllRequestCompress(PREQUEST_HEADER Header);
IRPMONDLL_API PREQUEST_HEADER WINAPI IRPMonDllRequestDecompress(const REQUEST_HEADER *Header);

IRPMONDLL_API DWORD WINAPI IRPMonDllEmulateDriverDevices(void);
IRPMONDLL_API DWORD WINAPI IRPMonDllEmulateProcesses(void);

IRPMONDLL_API DWORD WINAPI IRPMonDllSettingsQuery(PIRPMNDRV_SETTINGS Settings);
IRPMONDLL_API DWORD WINAPI IRPMonDllSettingsSet(PIRPMNDRV_SETTINGS Settings, BOOLEAN Save);


/************************************************************************/
/*           INITIALIZATION AND FINALIZATION                            */
/************************************************************************/


/** @brief
 *  Checks whether the IRPMon library is initialized.
 *
 *  @return
 *  Returns a boolean value indicating whether the library is initialized.
 *
 *  @remark
 *  The library is considered initialized if and only if a connection to the IRPMon
 *  driver device is established.
 */
IRPMONDLL_API BOOL WINAPI IRPMonDllInitialized(VOID);


/** Initializes the IRPMon library and connects the current process to the
 *  IRPMon driver.
 *
 *  @return
 *  Returns one of the following error codes:
 *  @value ERROR_SUCCESS The operation succeeded.
 *  @value Other The initialization failed. No other library functions may be
 *  called.
 *
 *  @remark
 *  This routine must be successfully called before any other routine exported
 *  by the library.
 */
IRPMONDLL_API DWORD WINAPI IRPMonDllInitialize(const IRPMON_INIT_INFO *Info);


/** Disconnects the current process from the IRPMon driver and cleans up
 *  resources used by the library.
 *
 *  @remark
 *  After calling this routine, no other routine exported by the library can
 *  be successfully invoked.
 */
IRPMONDLL_API VOID WINAPI IRPMonDllFinalize(VOID);


#endif 
