# EuhatExpert
This is an open source code of Euhat Expert.

## Official site:　
[http://euhat.com/expert/index.php](http://euhat.com/expert/index.php) 

## It has features:
* Transfer data between Android and PC.
* Work Trace Visualization.
* Clock In & Clock Out Inform.

## Important
1. Current building in Windows only supports x86!
2. For security, the release binary packages on official site use DB_KEYs which are different from the open source edition, so if you run the executable built from open source and the euhat.dat was generated by official executable, the euhat.dat will be clean and re-generate, all old user data will be lost.

## How to build:
1. In euhat\dbop\DbOpSqlite.cpp, modify DB_KEY_x to your private key of sqlite db, or else others will know to decrypt your certificate keys.
1. Open Android Studio, import project using build.gradle in android\EuhatExpert dir.
2. After install vs2019, double-click Expert.sln in msvc\EuhatExpert, compile all in x86 mode.

## About 3rd open sources
1. jpeg-9c
2. openssl

Have any question, welcome to contact me through email: euhat@hotmail.com
