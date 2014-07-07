#include <node.h>
#include <v8.h>
#include <Security/Security.h>
#include <SystemConfiguration/SystemConfiguration.h>


v8::Handle<v8::Value> LaunchPrivilegedProcess(const v8::Arguments& args) {
    v8::HandleScope scope;
    if(args.Length() != 1) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
      return scope.Close(v8::Undefined());
    }
    if(!args[0]->IsString()) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Argument must be a String")));
      return scope.Close(v8::Undefined());
    }
    v8::String::Utf8Value path(args[0]->ToString());
    AuthorizationRef authRef;

    OSStatus status;
    status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
    if (status == errAuthorizationSuccess) {
      status = AuthorizationExecuteWithPrivileges(authRef, (char *)*path, kAuthorizationFlagDefaults, NULL, NULL);
      AuthorizationFree(authRef, kAuthorizationFlagDestroyRights);
    }
    return scope.Close(v8::Boolean::New(status == errAuthorizationSuccess));
}

/*
 * For future use (eventually)
 *

v8::Handle<v8::Value> V8StringFromCFString(CFStringRef cfString) {
  if (!cfString) {
    return v8::Null();
  }
  CFIndex length = CFStringGetLength(cfString);
  CFIndex bufferSize = length + 1;
  char *cString = (char *)malloc(bufferSize);
  CFStringGetCString(cfString, cString, bufferSize, kCFStringEncodingASCII);
  v8::Handle<v8::String> v8String = v8::String::New(cString);
  free(cString);
  return v8String;
}

CFStringRef V8StringToCFString(v8::Handle<v8::Value> value) {
  if(value.IsEmpty() || !value->IsString()) {
    return NULL;
  }

  v8::String::AsciiValue string(value);

  return CFStringCreateWithCString(NULL, *string, kCFStringEncodingASCII);

}

void DictBoolValueToProp(v8::Handle<v8::Object> obj, CFDictionaryRef dict, CFStringRef key, const char *propName) {
  int val;
  CFNumberRef cfval = (CFNumberRef) CFDictionaryGetValue(dict, key);
  obj->Set(v8::String::NewSymbol(propName), v8::Boolean::New(cfval && CFNumberGetValue(cfval, kCFNumberIntType, &val) && (val != 0)));
}

void DictIntValueToProp(v8::Handle<v8::Object> obj, CFDictionaryRef dict, CFStringRef key, const char *propName) {
  int val = -1;
  CFNumberRef cfval = (CFNumberRef)CFDictionaryGetValue(dict, key);
  if(cfval) {
    CFNumberGetValue(cfval, kCFNumberIntType, &val);
  }
  obj->Set(v8::String::NewSymbol(propName), v8::Integer::New(val));
}

void DictStringValueToProp(v8::Handle<v8::Object> obj, CFDictionaryRef dict, CFStringRef key, const char *propName) {
  CFStringRef cfval = (CFStringRef) CFDictionaryGetValue(dict, key);
  obj->Set(v8::String::NewSymbol(propName), V8StringFromCFString(cfval));
}



v8::Handle<v8::Value> GetProxySettings(const v8::Arguments& args)
{
  v8::HandleScope scope;
  // Get the dictionary.

  SCDynamicStoreRef proxyStore = SCDynamicStoreCreate(NULL,CFSTR("node"),NULL,NULL);

  CFDictionaryRef  proxyDict = SCDynamicStoreCopyProxies(proxyStore);

  v8::Local<v8::Object> proxySettings = v8::Object::New();
  //HTTP
  DictBoolValueToProp(proxySettings, proxyDict, kSCPropNetProxiesHTTPEnable, "HTTPProxyEnabled");
  DictStringValueToProp(proxySettings, proxyDict, kSCPropNetProxiesHTTPProxy, "HTTPProxyHost");
  DictIntValueToProp(proxySettings, proxyDict, kSCPropNetProxiesHTTPPort, "HTTPProxyPort");

  //HTTPS
  DictBoolValueToProp(proxySettings, proxyDict, kSCPropNetProxiesHTTPSEnable, "HTTPSProxyEnabled");
  DictStringValueToProp(proxySettings, proxyDict, kSCPropNetProxiesHTTPSProxy, "HTTPSProxyHost");
  DictIntValueToProp(proxySettings, proxyDict, kSCPropNetProxiesHTTPSPort, "HTTPSProxyPort");

  //FTP
  DictBoolValueToProp(proxySettings, proxyDict, kSCPropNetProxiesFTPEnable, "FTPProxyEnabled");
  DictBoolValueToProp(proxySettings, proxyDict, kSCPropNetProxiesFTPPassive, "FTPProxyPassive");
  DictStringValueToProp(proxySettings, proxyDict, kSCPropNetProxiesFTPProxy, "FTPProxyHost");
  DictIntValueToProp(proxySettings, proxyDict, kSCPropNetProxiesFTPPort, "FTPProxyPort");

  if (proxyDict != NULL) {
    CFRelease(proxyDict);
  }

    // Get the enable flag.  This isn't a CFBoolean, but a CFNumber.

//             if (result) {
//                 enableNum = (CFNumberRef) CFDictionaryGetValue(proxyDict, kSCPropNetProxiesHTTPSEnable);

//                 result = (enableNum != NULL) && (CFGetTypeID(enableNum) == CFNumberGetTypeID());
//     }

//     if (result) {
//         result = CFNumberGetValue(enableNum, kCFNumberIntType, &enable) && (enable != 0);
//     }

//     // Get the proxy host.  DNS names must be in ASCII.  If you
//     // put a non-ASCII character  in the "Secure Web Proxy"
//     // field in the Network preferences panel, the CFStringGetCString
//     // function will fail and this function will return false.

//     if (result) {
//         hostStr = (CFStringRef) CFDictionaryGetValue(proxyDict, kSCPropNetProxiesHTTPSProxy);

//         result = (hostStr != NULL) && (CFGetTypeID(hostStr) == CFStringGetTypeID());
//     }

//     if (result) {
//         result = CFStringGetCString(hostStr, host, (CFIndex) hostSize, kCFStringEncodingASCII);
//     }

//     // Get the proxy port.

//     if (result) {
//         portNum = (CFNumberRef) CFDictionaryGetValue(proxyDict, kSCPropNetProxiesHTTPSPort);

//         result = (portNum != NULL) && (CFGetTypeID(portNum) == CFNumberGetTypeID());
//     }
//     if (result) {
//         result = CFNumberGetValue(portNum, kCFNumberIntType, &portInt);
//     }
//     if (result) {
//         *port = (UInt16) portInt;
//     }

//     //proxyDictSet = SCDynamicStoreCopyProxies(NULL);
//     proxyDictSet = CFDictionaryCreateMutableCopy(NULL,0,proxyDict);
//     CFDictionarySetValue(proxyDictSet, kSCPropNetProxiesHTTPSProxy,CFSTR("127.0.0.1"));
//     enable = 1;
//     CFDictionarySetValue(proxyDictSet, kSCPropNetProxiesHTTPSEnable,CFNumberCreate(NULL,kCFNumberLongType,&enable));
//     hostStr = (CFStringRef) CFDictionaryGetValue(proxyDictSet, kSCPropNetProxiesHTTPSProxy);
//     result = CFStringGetCString(hostStr, host, (CFIndex) hostSize, kCFStringEncodingASCII);
//     printf("HTTPS-Set proxy host = %s\n",host);

//     printf("now we try the new thing...\n");

//     //if(SCDynamicStoreSetValue(NULL,kSCPropNetProxiesHTTPSProxy,CFSTR("127.0.0.1")))
//     //{

//     if(SCDynamicStoreSetValue(proxyStore,kSCPropNetProxiesHTTPSProxy,proxyDictSet))
//     {
//       printf("store updated successfully...\n");
//     }else {
//       printf("store NOT updated successfully...\n");
//       printf("Error is %s\n",SCErrorString(SCError()));
//     }

//     // Clean up.

//     if (proxyDict != NULL) {
//       CFRelease(proxyDict);
//     }
//     if ( ! result ) {
//       *host = 0;
//       *port = 0;
//     }
//     return result;
  return scope.Close(proxySettings);
}

v8::Handle<v8::Value> SetProxySettings(const v8::Arguments& args)
{
  v8::HandleScope scope;
  if(args.Length() != 1) {
    v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
    return scope.Close(v8::Undefined());
  }
  if(!args[0]->IsObject()) {
    v8::ThrowException(v8::Exception::TypeError(v8::String::New("Argument must be an Object")));
    return scope.Close(v8::Undefined());
  }

  v8::Local<v8::Object> proxySettings = args[0]->ToObject();

  SCDynamicStoreRef proxyStore = SCDynamicStoreCreate(NULL,CFSTR("node"),NULL,NULL);

  CFDictionaryRef proxyDict = SCDynamicStoreCopyProxies(proxyStore);

  CFMutableDictionaryRef proxyDictSet = CFDictionaryCreateMutableCopy(NULL,0,proxyDict);

  v8::Local<v8::Value> result;

  int one = 1;
  CFNumberRef cfone = CFNumberCreate(NULL,kCFNumberLongType,&one);
  int port = 8888;
  CFNumberRef cfport = CFNumberCreate(NULL,kCFNumberLongType,&port);
  CFStringRef errDesc;
  CFIndex errCode;
  CFErrorRef err;

  CFStringRef hostStr = V8StringToCFString(proxySettings->Get(v8::String::NewSymbol("HTTPProxyHost")));
  if(hostStr) {
    CFDictionarySetValue(proxyDictSet, kSCPropNetProxiesHTTPProxy,hostStr);
    CFDictionarySetValue(proxyDictSet, kSCPropNetProxiesHTTPPort ,cfport);
    CFDictionarySetValue(proxyDictSet, kSCPropNetProxiesHTTPEnable,cfone);
    if(!SCDynamicStoreSetValue(proxyStore,kSCPropNetProxiesHTTPSProxy,proxyDictSet)) {
      err = SCCopyLastError();
      //errDesc = CFErrorCopyDescription(err);
      errCode = CFErrorGetCode(err);
      result = v8::Integer::New(errCode);
    }
  }



  //CFShow(proxyDictSet);

  if (proxyDict != NULL) {
    CFRelease(proxyDict);
  }

  return scope.Close(result);
//   return scope.Close(v8::Undefined());
}

*/

void Init(v8::Handle<v8::Object> exports) {
    exports->Set(v8::String::New("launchPriv"),
         v8::FunctionTemplate::New(LaunchPrivilegedProcess)->GetFunction());
//     exports->Set(v8::String::New("getProxySettings"),
//          v8::FunctionTemplate::New(GetProxySettings)->GetFunction());
//     exports->Set(v8::String::New("setProxySettings"),
//          v8::FunctionTemplate::New(SetProxySettings)->GetFunction());
}

NODE_MODULE(launch_priv, Init)
