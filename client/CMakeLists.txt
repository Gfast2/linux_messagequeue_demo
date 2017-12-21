cmake_minimum_required(VERSION 2.8.9)

project(client_msq)

add_executable(client_msq msq_header.h client_msq.c)
