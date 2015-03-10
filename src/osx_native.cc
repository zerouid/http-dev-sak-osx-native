#include <nan.h>
#include <Security/Security.h>
#include <SystemConfiguration/SystemConfiguration.h>

class PipeWorker : public NanAsyncProgressWorker {
    public:
        PipeWorker(
            NanCallback *callback,
            NanCallback *progress,
            FILE *pipe) : NanAsyncProgressWorker(callback), progress(progress), pipe(pipe) {}
        ~PipeWorker() {
            fclose(pipe);
        }

    void Execute (const NanAsyncProgressWorker::ExecutionProgress& progress) {
        int bufferSize = 256;
        char buffer[bufferSize];

        if(pipe) {
            for(;;) {
                int bytesRead = read (fileno (pipe), buffer, bufferSize);
                if (bytesRead < 1)
                    break;
                else
                    progress.Send(buffer, bytesRead);
            }
        }
    }

    void HandleProgressCallback(const char *data, size_t size) {
        NanScope();

        v8::Local<v8::Value> argv[] = {
            NanNewBufferHandle(data, size)
        };

        progress->Call(1, argv);
    }

    private:
        NanCallback *progress;
        FILE *pipe;
};


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

    if(args.Length() > 1 && !args[1]->IsArray()) {
        NanThrowTypeError("Argument must be a Array");
        NanReturnUndefined();
    }

    NanCallback *progress = NULL;
    if(args.Length() > 2 && args[2]->IsFunction()) {
        progress = new NanCallback(args[2].As<v8::Function>());
    }

    NanCallback *callback = NULL;
    if(args.Length() > 3 && args[3]->IsFunction()) {
        callback = new NanCallback(args[3].As<v8::Function>());
    }

    char **myArguments = NULL;
    FILE *myCommunicationsPipe = NULL;


    NanUtf8String path(args[0]->ToString());
    if(args.Length() >= 2) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(args[1]);
        int length = arr->Length();

        myArguments = (char**)malloc((length + 1) * sizeof(char*));
        for(int i = 0; i < length; i++) {
            NanAsciiString strval(arr->Get(i)->ToString());
            myArguments[i] = (char *) malloc(strval.length() + 1);
            strcpy(myArguments[i], *strval);
        }
        myArguments[length] = NULL;
    }

    AuthorizationRef authRef;

    OSStatus status = AuthorizationCreate(NULL,
                                          kAuthorizationEmptyEnvironment,
                                          kAuthorizationFlagDefaults,
                                          &authRef);

    if (status == errAuthorizationSuccess) {
        AuthorizationItem items = {kAuthorizationRightExecute, 0, NULL, 0};
        AuthorizationRights rights = {1, &items};

        status = AuthorizationCopyRights(authRef, &rights, NULL,
                                      kAuthorizationFlagDefaults |
                                      kAuthorizationFlagInteractionAllowed |
                                      kAuthorizationFlagPreAuthorize |
                                      kAuthorizationFlagExtendRights, NULL );

        if (status == errAuthorizationSuccess) {
            status = AuthorizationExecuteWithPrivileges(authRef,
                                                        (char *)*path,
                                                        kAuthorizationFlagDefaults,
                                                        myArguments,
                                                        &myCommunicationsPipe);

            if (status == errAuthorizationSuccess && callback != NULL && progress != NULL) {
                NanAsyncQueueWorker(new PipeWorker(callback, progress, myCommunicationsPipe));
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
