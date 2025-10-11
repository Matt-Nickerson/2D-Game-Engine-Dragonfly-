#pragma once
#include "Manager.h"

namespace df {

	class InputManager : public Manager {
	private:
		InputManager();                            
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

		// Mutable so getInput() can be const per the spec.
		mutable bool  m_prev_keys[256]{};
		mutable bool  m_prev_lb{ false };
		mutable bool  m_prev_rb{ false };
		mutable bool  m_prev_mb{ false };
		mutable long  m_prev_x{ 0 };
		mutable long  m_prev_y{ 0 };

	public:
		// Get the one and only instance of the InputManager.
		static InputManager& getInstance();

		// Get window ready to capture input. Return 0 if ok, else -1.
		int startUp() override;

		// Revert back to normal window mode.
		void shutDown() override;

		// Get input from the keyboard and mouse. Pass events to all Objects.
		void getInput() const;
	};

} 
