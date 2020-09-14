#include  "MemoryUtils.h"


#define    SCAN_PHYSICAL_MEMORY  	CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define    SPOOF_SMBIOS           	CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_ANY_ACCESS)





UNICODE_STRING      symLink;







NTSTATUS     unsupported(PDEVICE_OBJECT device_obj, PIRP irp) {
    UNREFERENCED_PARAMETER(device_obj);

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
}





NTSTATUS     CreateHandler(PDEVICE_OBJECT device_obj, PIRP irp) {
    UNREFERENCED_PARAMETER(device_obj);

    IoCompleteRequest(irp, IO_NO_INCREMENT);

}






NTSTATUS     CloseHandler(PDEVICE_OBJECT device_obj, PIRP irp) {
    UNREFERENCED_PARAMETER(device_obj);

    IoCompleteRequest(irp, IO_NO_INCREMENT);

}










NTSTATUS        IOCTLdispatch(DEVICE_OBJECT* DeviceObject, PIRP	    Irp)
{
    PIO_STACK_LOCATION		currentStackLocation = IoGetCurrentIrpStackLocation(Irp);

    INPUT_STRUCT* systemBuffer = (INPUT_STRUCT*)Irp->AssociatedIrp.SystemBuffer;





    switch (currentStackLocation->Parameters.DeviceIoControl.IoControlCode)
    {
    case    SCAN_PHYSICAL_MEMORY:
    {
        if (systemBuffer->wide == true)
        {
            DbgPrint("wide char: our pattern is %wZ \n", systemBuffer->serialNumber);
        }
        else
        {
            DbgPrint("normal: our pattern is %s \n", systemBuffer->serialNumber);
        }

        DbgPrint("our pattern length is %i \n", systemBuffer->serialLength);


        memcpy(Globals::spoofString, systemBuffer->spoofString, systemBuffer->serialLength);

        Memory::scanPhysicalMemory(systemBuffer);

        break;
    }
    case    SPOOF_SMBIOS:
    {
        ULONG       ntosSize;
        PVOID64     ntosBase = Utils::getDriverBaseAddress(&ntosSize, "ntoskrnl.exe");

        PPHYSICAL_ADDRESS WmipSMBiosTablePhysicalAddress = (PPHYSICAL_ADDRESS)Utils::findPattern((*)"\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x00\x8B\x15",
            14, (BYTE)"\x00", (ULONG64)ntosBase, ((ULONG64)ntosBase + ntosSize));

        if (WmipSMBiosTablePhysicalAddress)
        {
            WmipSMBiosTablePhysicalAddress = (PPHYSICAL_ADDRESS)Utils::ResolveRelativeAddress(WmipSMBiosTablePhysicalAddress, 3, 7);

            RtlZeroMemory(WmipSMBiosTablePhysicalAddress, sizeof(PHYSICAL_ADDRESS));
        }
    }
    default:
        break;
    }



    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
}









NTSTATUS    driverUnload(PDRIVER_OBJECT pDriverObject)
{
    DbgPrintEx(0, 0, "Unload routine called.\n");


    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(pDriverObject->DeviceObject);

 
}







NTSTATUS    Entry(_In_ _DRIVER_OBJECT * DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS            status;

    UNICODE_STRING      deviceName;

    PDEVICE_OBJECT      deviceObject;

    status = STATUS_SUCCESS;




    RtlInitUnicodeString(&deviceName, L"\\Device\\xPhymAqg");
    

    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, 0, &deviceObject);



    RtlInitUnicodeString(&symLink, L"\\DosDevices\\xPhymAqg");

    IoCreateSymbolicLink(&symLink, &deviceName);


    for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
    {
        DriverObject->MajorFunction[i] = unsupported;
    }


    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOCTLdispatch;

    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateHandler;

    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseHandler;


    DriverObject->DriverUnload = (PDRIVER_UNLOAD)&driverUnload;

    if (deviceObject)
    {
        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    }




    Globals::pageManager.initializePages();

}






NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registry_path) 
{
    DbgPrint("driver start \n");

    UNICODE_STRING      driverName;
    RtlInitUnicodeString(&driverName, L"\\Driver\\xPhymAqg");


    IoCreateDriver(&driverName, &Entry);

}