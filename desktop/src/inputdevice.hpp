namespace retroboy {
  enum class Button {LEFT, RIGHT, UP, DOWN, START, SELECT, A, B, GUIDE};

  class InputDevice {
  public:
    bool IsDown(Button button);
  };
}
