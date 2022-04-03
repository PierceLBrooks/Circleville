import os
import sys
import platform
import subprocess

def run(arguments):
  if (len(arguments) < 1):
    return -1
  command = ["git", "submodule", "update", "--init", "--recursive"]
  result = subprocess.check_output(command)
  print(result.decode())
  sdk = os.environ["ANDROID_SDK"]
  ndk = os.environ["ANDROID_NDK"]
  prefix = "./"
  suffix = ""
  if (platform.system() == "Windows"):
    sdk = sdk.replace("/", "\\").replace("\\", "\\\\").replace(":", "\\:")
    ndk = ndk.replace("/", "\\").replace("\\", "\\\\").replace(":", "\\:")
    suffix = ".bat"
    prefix = ""
  handle = open(os.path.join(os.path.dirname(arguments[0]), "Android-Modern", "local.properties"), "w")
  handle.write("sdk.dir="+sdk+"\n")
  handle.write("ndk.dir="+ndk+"\n")
  handle.close()
  command = [prefix+"gradlew"+suffix, 'assemble']
  result = subprocess.check_output(command, cwd=os.path.join(os.path.dirname(arguments[0]), "Android-Modern"))
  print(result.decode())
  return 0
  
def launch():
  result = run(sys.argv)
  print(str(result))
  return result

if (__name__ == "__main__"):
  launch()
  
