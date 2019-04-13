#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <gtest/gtest.h>

class Tests : public ::testing::Test {
};

TEST_F(Tests, ForARainyDay)
{
  QRegularExpression expr("(\\/\\/.*$)|(\\/\\*((.|\\n|\\r)*)\\*\\/)");
  const QString      text = "/* a\n * asdf\n */";

  QRegularExpressionMatchIterator it  = expr.globalMatch(text);
  QRegularExpressionMatch         m   = it.peekNext();
  QString                         cap = m.captured();

  ASSERT_TRUE(cap == text);
  ASSERT_TRUE(it.hasNext());
}
