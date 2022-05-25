namespace cmd {
	void createFile(const char* path, int size);
	void deleteFile(const char* path);
	void createDir(const char* path);
	void deleteDir(const char* path);
	void changeDir(const char* path);
	void dir();
	void cp(const char* src, const char* des);
	void sum();
	void cat(const char* path);
}

void commandInitializer();
