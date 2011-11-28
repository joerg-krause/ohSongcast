#include "Songcast.h"
#include "Icon.h"

#include <OpenHome/Private/Debug.h>
#include <algorithm>

using namespace OpenHome;
using namespace OpenHome::Net;


// C interface

uint32_t STDCALL SongcastSubnet(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetSubnet());
}

uint32_t STDCALL SongcastChannel(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetChannel());
}

uint32_t STDCALL SongcastTtl(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetTtl());
}

uint32_t STDCALL SongcastLatency(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetLatency());
}

uint32_t STDCALL SongcastMulticast(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetMulticast() ? 1 : 0);
}

uint32_t STDCALL SongcastEnabled(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetEnabled() ? 1 : 0);
}

uint32_t STDCALL SongcastPreset(THandle aSongcast)
{
	return (((Songcast*)aSongcast)->GetPreset());
}

void STDCALL SongcastSetSubnet(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetSubnet(aValue);
}

void STDCALL SongcastSetChannel(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetChannel(aValue);
}

void STDCALL SongcastSetTtl(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetTtl(aValue);
}

void STDCALL SongcastSetLatency(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetLatency(aValue);
}

void STDCALL SongcastSetMulticast(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetMulticast((aValue == 0) ? false : true);
}

void STDCALL SongcastSetEnabled(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetEnabled((aValue == 0) ? false : true);
}

void STDCALL SongcastSetPreset(THandle aSongcast, uint32_t aValue)
{
	((Songcast*)aSongcast)->SetPreset(aValue);
}

void STDCALL SongcastSetTrack(THandle aSongcast, const char* aUri, const char* aMetadata, uint64_t aSamplesTotal, uint64_t aSampleStart)
{
	((Songcast*)aSongcast)->SetTrack(aUri, aMetadata, aSamplesTotal, aSampleStart);
}

void STDCALL SongcastSetMetatext(THandle aSongcast, const char* aValue)
{
	((Songcast*)aSongcast)->SetMetatext(aValue);
}

void STDCALL SongcastRefreshReceivers(THandle aSongcast)
{
	((Songcast*)aSongcast)->RefreshReceivers();
}

void STDCALL SongcastDestroy(THandle aSongcast)
{
	delete ((Songcast*)aSongcast);
}

const char* STDCALL ReceiverUdn(THandle aReceiver)
{
	return (((Receiver*)aReceiver)->Udn());
}

const char* STDCALL ReceiverRoom(THandle aReceiver)
{
	return (((Receiver*)aReceiver)->Room());
}

const char* STDCALL ReceiverGroup(THandle aReceiver)
{
	return (((Receiver*)aReceiver)->Group());
}

const char* STDCALL ReceiverName(THandle aReceiver)
{
	return (((Receiver*)aReceiver)->Name());
}

EReceiverStatus STDCALL ReceiverStatus(THandle aReceiver)
{
	return (((Receiver*)aReceiver)->Status());
}

void STDCALL ReceiverPlay(THandle aReceiver)
{
	((Receiver*)aReceiver)->Play();
}

void STDCALL ReceiverStop(THandle aReceiver)
{
	((Receiver*)aReceiver)->Stop();
}

void STDCALL ReceiverStandby(THandle aReceiver)
{
	((Receiver*)aReceiver)->Standby();
}

void STDCALL ReceiverAddRef(THandle aReceiver)
{
	((Receiver*)aReceiver)->AddRef();
}

void STDCALL ReceiverRemoveRef(THandle aReceiver)
{
	((Receiver*)aReceiver)->RemoveRef();
}

uint32_t STDCALL SubnetAddress(THandle aSubnet)
{
	return (((Subnet*)aSubnet)->Address());
}

const char* STDCALL SubnetAdapterName(THandle aSubnet)
{
	return (((Subnet*)aSubnet)->AdapterName());
}

void STDCALL SubnetAddRef(THandle aSubnet)
{
	((Subnet*)aSubnet)->AddRef();
}

void STDCALL SubnetRemoveRef(THandle aSubnet)
{
	((Subnet*)aSubnet)->RemoveRef();
}


// Receiver

Receiver::Receiver(ReceiverManager3Receiver& aReceiver)
	: iReceiver(aReceiver)
	, iUdn(iReceiver.Udn())
	, iRoom(iReceiver.Room())
	, iGroup(iReceiver.Group())
	, iName(iReceiver.Name())
	, iRefCount(1)
{
	iReceiver.AddRef();
}

const TChar* Receiver::Udn() const
{
	return (iUdn.CString());
}

const TChar* Receiver::Room() const
{
	return (iRoom.CString());
}

const TChar* Receiver::Group() const
{
	return (iGroup.CString());
}

const TChar* Receiver::Name() const
{
	return (iName.CString());
}

EReceiverStatus Receiver::Status() const
{
	return (EReceiverStatus)iReceiver.Status();
}


void Receiver::Play()
{
	iReceiver.Play();
}

void Receiver::Stop()
{
	iReceiver.Stop();
}

void Receiver::Standby()
{
	iReceiver.Standby();
}

void Receiver::AddRef()
{
	iRefCount++;
}

void Receiver::RemoveRef()
{
	if (--iRefCount == 0) {
		delete (this);
	}
}

Receiver::~Receiver()
{
	iReceiver.RemoveRef();
}

// Subnet

Subnet::Subnet(NetworkAdapter& aAdapter)
	: iAdapter(&aAdapter)
    , iSubnet(aAdapter.Subnet())
{
	AddRef();
}

Subnet::Subnet(TIpAddress aSubnet)
	: iAdapter(0)
	, iSubnet(aSubnet)
{
}

TBool Subnet::IsAttachedTo(NetworkAdapter& aAdapter)
{
	if (iAdapter != 0) {
		return (iAdapter->Address() == aAdapter.Address());
	}
	return (false);
}

void Subnet::Attach(NetworkAdapter& aAdapter)
{
	RemoveRef();
	iAdapter = &aAdapter;
	AddRef();
    ASSERT(iAdapter->Subnet() == iSubnet);
}

void Subnet::Detach()
{
	RemoveRef();
    iAdapter = 0;
}

TIpAddress Subnet::Address() const
{
	if (iAdapter != 0) {
		return (iAdapter->Subnet());
	}

	return (iSubnet);
}

TIpAddress Subnet::AdapterAddress() const
{
	if (iAdapter != 0) {
		return (iAdapter->Address());
	}

	return (0);
}

const TChar* Subnet::AdapterName() const
{
	if (iAdapter != 0) {
		return (iAdapter->Name());
	}

	return ("Network adapter not present");
}

void Subnet::AddRef()
{
	if (iAdapter != 0) {
		iAdapter->AddRef();
	}
}

void Subnet::RemoveRef()
{
	if (iAdapter != 0) {
		iAdapter->RemoveRef();
	}
}

Subnet::~Subnet()
{
	RemoveRef();
}
    
// Songcast

Songcast::Songcast(TIpAddress aSubnet, TUint aChannel, TUint aTtl, TUint aLatency, TBool aMulticast, TBool aEnabled, TUint aPreset, ReceiverCallback aReceiverCallback, void* aReceiverPtr, SubnetCallback aSubnetCallback, void* aSubnetPtr, ConfigurationChangedCallback aConfigurationChangedCallback, void* aConfigurationChangedPtr, const Brx& aComputer, IOhmSenderDriver* aDriver, const char* aManufacturer, const char* aManufacturerUrl, const char* aModelUrl, const Brx& aImage, const Brx& aMimeType)
	: iSubnet(aSubnet)
	, iChannel(aChannel)
	, iTtl(aTtl)
	, iLatency(aLatency)
	, iMulticast(aMulticast)
	, iEnabled(aEnabled)
	, iPreset(aPreset)
	, iReceiverCallback(aReceiverCallback)
	, iReceiverPtr(aReceiverPtr)
	, iSubnetCallback(aSubnetCallback)
	, iSubnetPtr(aSubnetPtr)
	, iConfigurationChangedCallback(aConfigurationChangedCallback)
	, iConfigurationChangedPtr(aConfigurationChangedPtr)
	, iMutex("SCRD")
	, iClosing(false)
	, iAdapter(0)
	, iSender(0)
    , iDriver(aDriver)
{
	//Debug::SetLevel(Debug::kMedia);

	Bws<kMaxUdnBytes> udn;
    Bws<kMaxUdnBytes> name;
	Bws<kMaxUdnBytes + 1> friendly;
    Bws<kMaxUdnBytes + 1> description;

    name.Replace(aComputer);

    // TODO: manufacturer will need to be parsed and spaces replaced with -

    udn.Replace(aManufacturer);
	udn.Append("-Songcast-");
	udn.Append(aComputer);

	friendly.Replace(udn);

    description.Replace(aManufacturer);
    description.Append(" Songcast");

	InitialisationParams* initParams = InitialisationParams::Create();

	/*
	FunctorMsg fatal = MakeFunctorMsg(*this, &Songcast::FatalErrorHandler);

	initParams->SetFatalErrorHandler(fatal);
	*/

	Functor callback = MakeFunctor(*this, &Songcast::SubnetListChanged);

	initParams->SetSubnetListChangedListener(callback);

	UpnpLibrary::Initialise(initParams);

	UpnpLibrary::StartCombined(iSubnet);

	iDevice = new DvDeviceStandard(udn);
    
	iDevice->SetAttribute("Upnp.Domain", "av.openhome.org");
    iDevice->SetAttribute("Upnp.Type", "Songcast");
    iDevice->SetAttribute("Upnp.Version", "1");
    iDevice->SetAttribute("Upnp.FriendlyName", (TChar*)friendly.PtrZ());
    iDevice->SetAttribute("Upnp.Manufacturer", (TChar*)description.PtrZ());
    iDevice->SetAttribute("Upnp.ManufacturerUrl", (TChar*)aManufacturerUrl);
    iDevice->SetAttribute("Upnp.ModelDescription", (TChar*)description.PtrZ());
    iDevice->SetAttribute("Upnp.ModelName", (TChar*)description.PtrZ());
    iDevice->SetAttribute("Upnp.ModelNumber", "1");
    iDevice->SetAttribute("Upnp.ModelUrl", (TChar*)aModelUrl);
    iDevice->SetAttribute("Upnp.SerialNumber", "");
    iDevice->SetAttribute("Upnp.Upc", "");

	SubnetListChanged();

	iSender = new OhmSender(*iDevice, *iDriver, name, iChannel, iAdapter, iTtl, iLatency, iMulticast, iEnabled, aImage, aMimeType, iPreset);

	iNetworkMonitor = new NetworkMonitor(*iDevice, name);
	
	iDevice->SetEnabled();

	iReceiverManager = new ReceiverManager3(*this, iSender->SenderUri(), iSender->SenderMetadata());
}


void Songcast::FatalErrorHandler(const char* /*aMessage*/)
{
}


/*
#include <Objbase.h>

struct MySEHExceptionStruct
{
  char* m_lpszMessage1;
  char m_szMessage2[256];
};

void Songcast::FatalErrorHandler(const char* aMessage)
{
 // First allocate space for a MySEHExceptionStruct instance.
  MySEHExceptionStruct* pMySEHExceptionStruct=(MySEHExceptionStruct*)::CoTaskMemAlloc(sizeof(MySEHExceptionStruct));
  // Zero out all bytes inside pMySEHExceptionStruct.
  memset (pMySEHExceptionStruct, 0, sizeof(MySEHExceptionStruct));

  // Assign value to the m_lpszMessage member.
  const char* lpszMessage1 = "SEH Exception Message 1.";
  const char* lpszMessage2 = "SEH Exception Message 2.";

  pMySEHExceptionStruct -> m_lpszMessage1 = (char*)::CoTaskMemAlloc(strlen(lpszMessage1) + 1);
  strcpy(pMySEHExceptionStruct -> m_lpszMessage1, lpszMessage1);
  strcpy(pMySEHExceptionStruct -> m_szMessage2, lpszMessage2);

  // Raise the SEH exception, passing along a ptr to the MySEHExceptionStruct
  // structure. Note that the onus is on the recipient of the exception to free
  // the memory of pMySEHExceptionStruct as well as its contents.
  RaiseException(100, 0, 1, (const ULONG_PTR*)(pMySEHExceptionStruct));
}
*/

// Simple predicate class for finding NetworkAdapter and Subnet objects with a given
// subnet TIpAddress in a list - simplfies the code in the SubnetListChanged method

class SubnetFinder
{
public:
    SubnetFinder(TIpAddress aSubnet) : iSubnet(aSubnet) {}

    bool operator()(NetworkAdapter* aAdapter) const {
        return (aAdapter->Subnet() == iSubnet);
    }

    bool operator()(Subnet* aSubnet) const {
        return (aSubnet->Address() == iSubnet);
    }

private:
    TIpAddress iSubnet;
};

// Don't bother removing old subnets - they might come back anyway, and there is not exactly
// a huge traffic in added and removed network interfaces

void Songcast::SubnetListChanged()
{
	iMutex.Wait();

	TBool closing = iClosing;

	iMutex.Signal();

	if (closing) {
		return;
	}

    // get the new subnet list from ohnet
    std::vector<NetworkAdapter*>* subnetList = UpnpLibrary::CreateSubnetList();

    std::vector<NetworkAdapter*>::iterator newSubnetListIt;
    std::vector<Subnet*>::iterator oldSubnetListIt;

    // look for new subnets that already exist in or need to be added to the current list 
    for (newSubnetListIt = subnetList->begin() ; newSubnetListIt != subnetList->end() ; newSubnetListIt++)
    {
        // iterator is the adapter to use for this new subnet
        NetworkAdapter* adapter = *newSubnetListIt;

        // look for this new subnet in the current subnet list
        oldSubnetListIt = std::find_if(iSubnetList.begin(), iSubnetList.end(), SubnetFinder(adapter->Subnet()));

        if (oldSubnetListIt != iSubnetList.end())
        {
            // the new subnet already exists in the current subnet list
            Subnet* subnet = *oldSubnetListIt;

            if (!subnet->IsAttachedTo(*adapter))
            {
                // the corresponding subnet in the current list is attached to a different adapter, so attach this new one
                subnet->Attach(*adapter);
                (*iSubnetCallback)(iSubnetPtr, eChanged, (THandle)subnet);
            }
        }
        else
        {
            // the new subnet is not in the current list - add it
            Subnet* subnet = new Subnet(*adapter);
            iSubnetList.push_back(subnet);
            (*iSubnetCallback)(iSubnetPtr, eAdded, (THandle)subnet);
        }
    }

    // now look for subnets in the current list that are absent from the new list
    for (oldSubnetListIt = iSubnetList.begin() ; oldSubnetListIt != iSubnetList.end() ; oldSubnetListIt++)
    {
        Subnet* subnet = *oldSubnetListIt;

        // look for this subnet in the new list
        if (std::find_if(subnetList->begin(), subnetList->end(), SubnetFinder(subnet->Address())) == subnetList->end())
        {
            // this subnet is not in the new list so detach it from its current adapter
            subnet->Detach();
            (*iSubnetCallback)(iSubnetPtr, eChanged, (THandle)subnet);
        }
    }

	UpnpLibrary::DestroySubnetList(subnetList);

	// Now manage our current subnet and adapter

	if (!UpdateAdapter()) {

		// Not found - make a dummy subnet entry to represent our current subnet (unless 0)

		if (iSubnet != 0)
		{
			Subnet* subnet = new Subnet(iSubnet);
			iSubnetList.push_back(subnet);
			(*iSubnetCallback)(iSubnetPtr, eAdded, (THandle)subnet);
		}
	}
}

// return true if the current subnet was found in the list

TBool Songcast::UpdateAdapter()
{
	std::vector<Subnet*>::iterator it = std::find_if(iSubnetList.begin(), iSubnetList.end(), SubnetFinder(iSubnet));

    if (it != iSubnetList.end())
    {
        // the subnet exists in the subnet list - update the adapter interface if necessary
        Subnet* subnet = *it;
        TIpAddress adapter = subnet->AdapterAddress();

        if (iAdapter != adapter) {
            iAdapter = adapter;

            if (iSender != 0) {
                iSender->SetInterface(iAdapter);
            }
        }
    }

    return (it != iSubnetList.end());
}

TIpAddress Songcast::GetSubnet()
{
	iMutex.Wait();
	TIpAddress subnet = iSubnet;
	iMutex.Signal();
	return (subnet);
}

TUint Songcast::GetChannel()
{
	iMutex.Wait();
	TUint channel = iChannel;
	iMutex.Signal();
	return (channel);
}

TUint Songcast::GetTtl()
{
	iMutex.Wait();
	TUint ttl = iTtl;
	iMutex.Signal();
	return (ttl);
}

TUint Songcast::GetLatency()
{
	iMutex.Wait();
	TUint latency = iLatency;
	iMutex.Signal();
	return (latency);
}

TBool Songcast::GetMulticast()
{
	iMutex.Wait();
	TBool multicast = iMulticast;
	iMutex.Signal();
	return (multicast);
}

TBool Songcast::GetEnabled()
{
	iMutex.Wait();
	TBool enabled = iEnabled;
	iMutex.Signal();
	return (enabled);
}

TUint Songcast::GetPreset()
{
	iMutex.Wait();
	TUint preset = iPreset;
	iMutex.Signal();
	return (preset);
}

void Songcast::SetSubnet(TIpAddress aValue)
{
	iMutex.Wait();

	if (iSubnet == aValue || iClosing) {
		iMutex.Signal();
		return;
	}

	iSubnet = aValue;

	iMutex.Signal();

	UpnpLibrary::SetCurrentSubnet(iSubnet);

	ASSERT(UpdateAdapter());

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetChannel(TUint aValue)
{
	iMutex.Wait();

	if (iChannel == aValue || iClosing) {
		iMutex.Signal();
		return;
	}

	iChannel = aValue;

	iMutex.Signal();

	iSender->SetChannel(aValue);

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetTtl(TUint aValue)
{
	iMutex.Wait();

	if (iTtl == aValue || iClosing) {
		iMutex.Signal();
		return;
	}

	iTtl = aValue;

	iMutex.Signal();

	iSender->SetTtl(aValue);

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetLatency(TUint aValue)
{
	iMutex.Wait();

	if (iLatency == aValue  || iClosing) {
		iMutex.Signal();
		return;
	}

	iLatency = aValue;

	iMutex.Signal();

	iSender->SetLatency(aValue);

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetMulticast(TBool aValue)
{
	iMutex.Wait();

	if (iMulticast == aValue || iClosing) {
		iMutex.Signal();
		return;
	}

	iMulticast = aValue;

	iMutex.Signal();

	iSender->SetMulticast(aValue);

	iReceiverManager->SetMetadata(iSender->SenderMetadata());

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetEnabled(TBool aValue)
{
	iMutex.Wait();

	if (iEnabled == aValue || iClosing) {
		iMutex.Signal();
		return;
	}

	iEnabled = aValue;

	iMutex.Signal();

	iSender->SetEnabled(aValue);

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetPreset(TUint aValue)
{
	iMutex.Wait();

	if (iPreset == aValue || iClosing) {
		iMutex.Signal();
		return;
	}

	iPreset = aValue;

	iMutex.Signal();

	iSender->SetPreset(aValue);

	(*iConfigurationChangedCallback)(iConfigurationChangedPtr, this);
}

void Songcast::SetTrack(const TChar* aUri, const TChar* aMetadata, TUint64 aSamplesTotal, TUint64 aSampleStart)
{
	iSender->SetTrack(Brn(aUri), Brn(aMetadata), aSamplesTotal, aSampleStart);
}

void Songcast::SetMetatext(const TChar* aValue)
{
	iSender->SetMetatext(Brn(aValue));
}

void Songcast::RefreshReceivers()
{
	iReceiverManager->Refresh();
}

Songcast::~Songcast()
{
    LOG(kMedia, "Songcast::~Songcast\n");

	iMutex.Wait();

	iClosing = true;

	iMutex.Signal();

    LOG(kMedia, "Songcast::~Songcast registered closing\n");

	delete (iReceiverManager);

    LOG(kMedia, "Songcast::~Songcast receiver manager destroyed\n");

	delete (iSender);

    LOG(kMedia, "Songcast::~Songcast sender destroyed\n");

	delete (iDevice);

    LOG(kMedia, "Songcast::~Songcast device destroyed\n");

	delete (iDriver);

    LOG(kMedia, "Songcast::~Songcast driver destroyed\n");

	std::vector<Subnet*>::iterator it = iSubnetList.begin();

	while (it != iSubnetList.end()) {
		Subnet* subnet = *it;
		(*iSubnetCallback)(iSubnetPtr, eRemoved, (THandle)subnet);
		delete (subnet);
		it++;
	}

    LOG(kMedia, "Songcast::~Songcast subnets destroyed\n");

	Net::UpnpLibrary::Close();

    LOG(kMedia, "Songcast::~Songcast library closed\n");
}

// IReceiverManager3Handler

void Songcast::ReceiverAdded(ReceiverManager3Receiver& aReceiver)
{
	Receiver* receiver = new Receiver(aReceiver);
	aReceiver.SetUserData(receiver);
	(*iReceiverCallback)(iReceiverPtr, eAdded, (THandle)receiver);
}

void Songcast::ReceiverChanged(ReceiverManager3Receiver& aReceiver)
{
	Receiver* receiver = (Receiver*)(aReceiver.UserData());
	ASSERT(receiver);
	(*iReceiverCallback)(iReceiverPtr, eChanged, (THandle)receiver);
}

void Songcast::ReceiverRemoved(ReceiverManager3Receiver& aReceiver)
{
	Receiver* receiver = (Receiver*)(aReceiver.UserData());
	ASSERT(receiver);
	(*iReceiverCallback)(iReceiverPtr, eRemoved, (THandle)receiver);
	receiver->RemoveRef();
}