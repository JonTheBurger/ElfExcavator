#ifndef PALETTE_HXX
#define PALETTE_HXX

#include <QColor>
#include <vector>

class Palette {
public:
  Palette();
  static QColor               RED;
  static QColor               ORANGE;
  static QColor               YELLOW;
  static QColor               GREEN;
  static QColor               BLUE;
  static QColor               PURPLE;
  static QColor               GRAY;
  static QColor               PINK;
  static QColor               GREEN_LIGHT;
  static QColor               BLUE_LIGHT;
  static QColor               BROWN;
  static std::vector<QColor*> COLORS;
};

#endif  // PALETTE_HXX
