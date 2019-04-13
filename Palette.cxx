#include "Palette.hxx"

Palette::Palette()
{
}

QColor Palette::RED(0xCC0000);
QColor Palette::ORANGE(0xFD971F);
QColor Palette::YELLOW(0xFFC66D);
QColor Palette::GREEN(0x1EAA1E);
QColor Palette::BROWN(0x685238);
QColor Palette::BLUE(0x0484FF);
QColor Palette::PURPLE(0xAE81FF);
QColor Palette::GRAY(0x808080);
QColor Palette::PINK(0xEE72BA);
QColor Palette::GREEN_LIGHT(0x4EF278);
QColor Palette::BLUE_LIGHT(0x66D9EF);

std::vector<QColor*> Palette::COLORS = {
  &Palette::GREEN_LIGHT,
  &Palette::BLUE,
  &Palette::ORANGE,
  &Palette::RED,
  &Palette::PURPLE,
  &Palette::BLUE_LIGHT,
  &Palette::PINK,
  &Palette::GRAY,
  &Palette::GREEN,
  &Palette::BROWN,
  &Palette::YELLOW,
};
