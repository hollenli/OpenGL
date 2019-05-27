#pragma once
#ifndef _HW_H_ 
#define _HW_H_ 

// settings

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

unsigned int load_hw2();
void render_hw2(unsigned int);

unsigned int load_hw3();
void render_hw3(unsigned int);

unsigned int load_hw4();
void render_hw4(unsigned int);

unsigned int load_hw5();
void render_hw5(unsigned int, GLFWwindow *);

void render_hw6();

void render_hw7();

void render_hw8();
void mousebutton_callback(GLFWwindow*, int, int, int);

#endif