#include <nan.h>
#include <Security/Security.h>
#include <SystemConfiguration/SystemConfiguration.h>


NAN_METHOD(LaunchPrivilegedProcess) {
  NanScope();
  if(args.Length() < 1 && args.Length() > 2) {
    NanThrowTypeError("Wrong number of arguments");
    NanReturnUndefined();
  }
  if(!args[0]->IsString()) {
    NanThrowTypeError("Argument must be a String");
    NanReturnUndefined();
  }
  if(args.Length() == 2 && !args[1]->IsArray()) {
    NanThrowTypeError("Argument must be a Array");
    NanReturnUndefined();
  }

  NanUtf8String path(args[0]->ToString());
  char **myArguments = NULL;
  FILE *myCommunicationsPipe = NULL;
  if(args.Length() == 2) {
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(args[1]);
    int length = arr->Length();
    myArguments = (char**)malloc((length + 1) * sizeof(char*));
    for(int i = 0; i < length; i++)
    {
      NanUtf8String strval(arr->Get(i)->ToString());
      myArguments[i] = (char *) malloc(strval.length() + 1);
      strcpy(myArguments[i], *strval);
    }
    myArguments[length] = NULL;
  }

  AuthorizationRef authRef;
  char myReadBuffer[128];

  OSStatus status;
  status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
  if (status == errAuthorizationSuccess) {
    AuthorizationItem items = {kAuthorizationRightExecute, 0, NULL, 0};
    AuthorizationRights rights = {1, &items};

    status = AuthorizationCopyRights (authRef, &rights, NULL, kAuthorizationFlagDefaults |
                                      kAuthorizationFlagInteractionAllowed |
                                      kAuthorizationFlagPreAuthorize |
                                      kAuthorizationFlagExtendRights, NULL );
    if (status == errAuthorizationSuccess) {
      status = AuthorizationExecuteWithPrivileges(authRef, (char *)*path, kAuthorizationFlagDefaults, myArguments, &myCommunicationsPipe);
      if (status == errAuthorizationSuccess)
        for(;;)
      {
        int bytesRead = read (fileno (myCommunicationsPipe),myReadBuffer, sizeof (myReadBuffer));
        if (bytesRead < 1) break;
        write (fileno (stdout), myReadBuffer, bytesRead);
      }
    }
  }

  if(myArguments) {
    for(int i=0; myArguments[i] != NULL; i++) {
      free(myArguments[i]);
    }

    free(myArguments);
  }
  AuthorizationFree(authRef, kAuthorizationFlagDefaults);
  NanReturnValue(NanNew(status));
}

void Init(v8::Handle<v8::Object> exports) {
  exports->Set(NanNew("launchPriv"),
               NanNew<v8::FunctionTemplate>(LaunchPrivilegedProcess)->GetFunction());
}

NODE_MODULE(launch_priv, Init)
