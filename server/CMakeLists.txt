cmake_minimum_required(VERSION 2.8.9)

project(msq_server)

add_executable(msq_server msq_header.h server_msq.c)
