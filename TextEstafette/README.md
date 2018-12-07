## Text Estafette
That project is about process communication via mutex.
Process TextEstafette.exe creates mutex, file(OPEN_EXISTING) and process  Subprocess.exe.
Pass handle of file via cmd(command line) and starts concurently work with subprocess.exe.
Processes wait until mutex will be free, via WaitForSingleObject(), and when process obtains ownersip ot the mutex, 
starts read from STD_INPUT and write in file which was created from textestafette.exe.


## Contribution
If you have any questions or offers do not hesitate to create issues and pull requests.
