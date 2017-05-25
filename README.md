# project_psis
Project for the course of Systems Programming
Instituto Superior Técnico
2016/17 1st Semester

gw: 
gcc -g gw.c gw_library.c list.c -o gw -lpthread

server: 
gcc -g server.c server_library.c list.c -o server -lpthread

client: 
gcc -g client.c library.c -o client
