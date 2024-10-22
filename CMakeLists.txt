cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 11)

add_executable(server server.c
        db.h
        admin/admintasks.h
        admin/admintasks.c
        customer/customertask.c
        customer/customertask.h
        customer/customertask.c
        customer/customertask.c
        employee/emptask.c
        employee/emptask.h
        manager/managertask.c
        manager/managertask.h
        login/login.c
        login/login.h

)
add_executable(client client.c)
add_executable(makeadmin makeadmin.c)