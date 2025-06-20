# 🛳️ DockerCLI Tool

A console utility that allows you to easily create Dockerfile and docker-compose with a single command.

## ⚙️ Installing

1) Download `dockercli.exe` from Releases or source code
2) Create any directory and put this file to there (e.x. `C:\bin\dockercli.exe`)
3) Press `Win + R`, type `sysdm.cpl`, go to `Additionally`, press `Environment Variables`
4) In opened window find `Path` -> `Edit` -> `Create` and press path to `dockercli.exe`
5) Press `Ok` in all windows

## 🧭 Usage

Open command line or Windows PowerShell and type `dockercli help` for view list of all commands. 

To create `Dockerfile` and `docker-compose.yml` in current directory, enter `dockercli generate [FROM] (params)`. For example: `dockercli g node --v21 --mysql --buildNode`. Output:

```Dockerfile
# Dockerfile
FROM node:21

WORKDIR /app

COPY package.json package-lock.json* ./

RUN npm install

COPY . .

RUN npm run build

CMD ["node", "dist/main.js"]
```

docker-compose.yml
```yaml
version: '3.8'

services:
  mysql:
    image: mysql:8.0
    command: --log-error-verbosity=2
    restart: always
    environment:
      MYSQL_ROOT_PASSWORD: passroot
      MYSQL_USER: msqluser
      MYSQL_PASSWORD: msqlpass
      MYSQL_DATABASE: app
    ports:
      - "3306:3306"
    volumes:
      - mysql_data:/var/lib/mysql
    healthcheck:
      test: [ "CMD", "mysqladmin", "ping", "-h", "localhost" ]
      interval: 5s
      timeout: 10s
      retries: 5

  app:
    depends_on:
      mysql:
        condition: service_healthy
    build: .
    tty: true

volumes:
  mysql_data:
```

## 📝 Docs

### 📨 All FROMs

- `node` - NodeJS
- `gcc` - C compiler
- `python` - Python

### 🏷️ All Options

- `--v*` - sets version of `FROM`
- `--mysql` - adds mysql image to `docker-compose.yml`
- `--nasm` - applies to gcc only. Will run .asm file via NASM
- `--nodeBuild` -- adds `RUN npm run build` in Dockerfile