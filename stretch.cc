#include <iostream>
#include <node.h>
#include <vector>
#include <uv.h>
#include <iomanip>
#include <sstream>

#include <string.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

int pbkdf2(const char *pass, const char *salt, long iter, const EVP_MD *digest, unsigned char *out)
{
  unsigned char digtmp[32], itmp[4];
  int k, mdlen, saltlen = strlen(salt);
  long j;
  HMAC_CTX hctx_tpl, hctx;

  mdlen = EVP_MD_size(digest);
  HMAC_CTX_init(&hctx_tpl);
  HMAC_Init_ex(&hctx_tpl, pass, strlen(pass), digest, NULL);
  itmp[0] = itmp[1] = itmp[2] = 0; itmp[3] = 1;
  HMAC_CTX_copy(&hctx, &hctx_tpl); HMAC_Update(&hctx, (const unsigned char*)salt, saltlen); HMAC_Update(&hctx, itmp, 4); HMAC_Final(&hctx, digtmp, NULL); HMAC_CTX_cleanup(&hctx);
  memcpy(out, digtmp, mdlen);
  for (j = 1; j < iter; j++) {
    HMAC_CTX_copy(&hctx, &hctx_tpl); HMAC_Update(&hctx, digtmp, mdlen); HMAC_Final(&hctx, digtmp, NULL); HMAC_CTX_cleanup(&hctx);
    for (k = 0; k < mdlen; k++) out[k] ^= digtmp[k];
  }
  HMAC_CTX_cleanup(&hctx_tpl);
  return 1;
}


namespace strech {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Function;

using v8::Persistent;


struct Work {
  uv_work_t request;
  Persistent<Function> callback;
  std::string hash;
  long iterations;
  std::string result;
};

static void WorkAsync(uv_work_t *req) {
  Work* work = static_cast<Work*>(req->data);

  unsigned char out[32];
  pbkdf2(work->hash.c_str(), "pevpot", work->iterations, EVP_sha256(), out);

  std::stringstream stream;

  stream << std::setfill ('0') << std::setw(2) << std::hex;
  for (int i=0;i<32;++i) { stream << ((int) out[i]); }

  work->result = stream.str();
}

static void WorkAsyncComplete(uv_work_t *req, int status) {
  Work* work = static_cast<Work*>(req->data);

  Isolate* isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);

  const unsigned argc = 2;
  Local<Value> argv[argc]= { Null(isolate), String::NewFromUtf8(isolate, work->result.c_str()) };

  Local<Function>::New(isolate, work->callback)->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong number of arguments")
    ));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Expected string for hash")));
    return;
  }

  if (!args[1]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Expected number for iterations")));
    return;
  }


  if (!args[2]->IsFunction()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Expected function for callback")));
    return;
  }

  Local<Function> cb = Local<Function>::Cast(args[2]);

  Work* work = new Work();
  work->request.data = work;
  work->hash = *v8::String::Utf8Value(args[0]);
  work->iterations = args[1]->NumberValue();
  work->callback.Reset(isolate, cb);

  // kick off worker thread
  uv_queue_work(uv_default_loop(), &work->request, WorkAsync, WorkAsyncComplete);
}

void init(Local<Object> exports, Local<Object> module) {
  NODE_SET_METHOD(module, "exports", Method);
}

NODE_MODULE(addon, init)

}  // namespace demo
