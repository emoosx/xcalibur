#ifndef testParser_h
#define testParser_h
#include <cppunit/extensions/HelperMacros.h>

class TestParser : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(TestParser);
	//CPPUNIT_TEST(testTotalStatementNumber);
	//CPPUNIT_TEST(testTotalFollows);
	//CPPUNIT_TEST(testTotalParents);
	//CPPUNIT_TEST(testTotalModifies);
	CPPUNIT_TEST(testTotalUses);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();
	void testTotalStatementNumber();
	void testTotalFollows();
	void testTotalParents();
	void testTotalUses();
	void testTotalModifies();
};
#endif