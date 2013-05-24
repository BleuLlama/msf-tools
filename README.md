msf-tools
=========

Some tools to interact with MSGLISTS.MSF message list files from a Sony ICD-PX820 voice recorder

---

All of the content in this has been the result of some reverse-engineering
of MSF files.  Currently the only tool is msf-dump, which will
display information about the MSF file.  It will display the 6
folders, as well as a list of their contents, including date/time
that they were recoreded.

It is far from complete.

Included here are a class (VoiceFile) which interfaces with the MSF file.
The source for this file has information about the discovered file structure.

Set the environment variable "VOICERECORDER" to point to the drive/mountpoint of the recorder.

Also here are two targets that will get built when you type 'make':

msf-dump
	Will load in the MSF file, and dump out a listing of its contents to stdout.
	It uses functions in VoiceFile to accomplish this.
	It is meant more as a demo of the API than anything.

msf-autosave <target directory>
	Will read in the MSF file, and copy the contents out of the recorder, 
	and save them into the target directory, sorted by folder name, 
	with filenames based on the recording date and time.


Note: I am not affiliated with Sony in any way.

- Scott Lawrence, 2013

---

@@ Version History

- 1.0 2013-May-23 - Initial release
