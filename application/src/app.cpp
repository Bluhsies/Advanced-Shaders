#include "app.hpp"
#include "startPoint.hpp"
#include "Lab1.hpp"
#include "Lab2.hpp"
#include "Lab2multiPass.hpp"
#include "Lab3.hpp"
#include "Lab4.hpp"
#include "Lab5.hpp"
#include "Lab6.hpp"
#include "Lab7.hpp"
#include "Lab8.hpp"
#include "Lab9.hpp"

App::App(const WindowProperties& winProps) : Application(winProps)
{
	//m_layer = std::unique_ptr<Layer>(new StartPoint(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab1(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab2(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab2multiPass(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab3(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab4(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab5(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab6(m_window));
	m_layer = std::unique_ptr<Layer>(new Lab7(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab8(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab9(m_window));



}

Application* startApplication()
{
	WindowProperties props("GAMR3521", 800, 600);
	props.isHostingImGui = true;
	props.isResizable = false;
	return new App(props);
}