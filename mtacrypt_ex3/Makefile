.PHONY: all clean build

all: clean build

clean:
	docker rm -f $$(docker ps -aq) 2>/dev/null || true
	docker rmi -f $$(docker images -aq --filter=reference='mtacrypt_*') 2>/dev/null || true
	rm -rf /tmp/mtacrypt /tmp/mtacrypt_logs

build:
	docker build -t mtacrypt_encrypter -f encrypter/Dockerfile .
	docker build -t mtacrypt_decrypter -f decrypter/Dockerfile .

