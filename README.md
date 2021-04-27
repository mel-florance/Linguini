# Linguini

[![GitHub license](https://img.shields.io/github/license/Vyraax/Linguini.svg)](https://github.com/Vyraax/Linguini/blob/master/LICENSE)&nbsp;
[![GitHub tag](https://img.shields.io/github/tag/Vyraax/Linguini.svg)](https://gitHub.com/Vyraax/Linguini/tags/)&nbsp;
![build](https://github.com/Vyraax/Linguini/workflows/Linux%20x64/badge.svg?branch=master)&nbsp;

Web Server features:
- [x] MVC workflow
- [x] ORM with mysql connector
- [x] QueryBuilder
- [x] Router
- [x] SMTP client (with multithread)
- [x] JSON support
- [x] Html templating support with Inja
- [x] Static files browsing and downloading / viewing
- [x] Logging system
- [x] User Agent parser
- [x] HTTP Basic Auth login system
- [x] Simple integrated web server status monitoring
- [ ] Multi websites support (in progress)
- [ ] Websockets (in progress)
- [ ] Json Web Token (in progress)

## Documentation
You can access the documentation directly on the wiki page:\
[https://github.com/Vyraax/Linguini/wiki](https://github.com/Vyraax/Linguini/wiki)

## Compile from sources

###### Ubuntu build

```console
sudo apt install libmysqlclient-dev libmysqlcppconn-dev libuv1-dev openssl
git clone https://github.com/Vyraax/Linguini.git
cd Linguini
./GenerateProject.sh
make
```

###### Start server

```console
./start.sh
```
## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.\
Please make sure to update tests as appropriate.

## License
[License](https://github.com/Vyraax/Linguini/tree/master/LICENSE)

