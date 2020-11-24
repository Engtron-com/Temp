#include "ScecpServer/Public/ServerHelper.h"
#include "iostream"

#include "Mariadb/Public/MariadbConnector.h"
#include "ScecpServer/Public/ScecpServerPCH.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "String/Public/StringHelper.h"
#include "cinatra.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/format.hpp"
#include "Core/Public/Vector.h"
#include "ScecpServer/Public/HeatMap/HeatMapHelper.h"
#include <math.h>
#include <boost/lexical_cast.hpp>    
#include <cmath>
#include <set>

#define FIELDIFNULL(X) X.is_null() ? "" : X.get<std::string>()
#define FIELDIFNULL_FLOAT(X) X.is_null() ? 0 : X.get<float>()
#define FIELDIFNULL_INT(X) X.is_null() ? 0 : X.get<int>()
#define TCHAR_TO_UTF8(X) UnicodeToUTF8(X)
#define UTF8_TO_TCHAR(X) UTF8ToUnicode(X)

std::vector<std::string> split(const std::string & str, const std::string & pattern)
{
	//const char* convert to char*
	char * strc = new char[strlen(str.c_str()) + 1];
	strcpy(strc, str.c_str());
	std::vector<std::string> resultVec;
	char * tmpStr = strtok(strc, pattern.c_str());
	while (tmpStr != NULL)
	{
		resultVec.push_back(std::string(tmpStr));
		tmpStr = strtok(NULL, pattern.c_str());
	}
	delete[] strc;
	return resultVec;
};
int find(const std::vector<std::string> & where, std::string s)
{
	//判断vector中是否存在某字符串，如果存在返回位置，否则返回-1
	for (int i = 0; i < where.size(); i++) {
		if (where[i] == s) {
			return i;
		}
	}
	return -1;
}
bool SortCategoryDatas(Json & InCategoryDataA, Json & InCategoryDataB)
{
	try
	{
		return boost::lexical_cast<double>(std::string(InCategoryDataA["yLabel"])) > boost::lexical_cast<double>(std::string(InCategoryDataB["yLabel"]));
	}
	catch (const std::exception &)
	{
		return false;
	}
}
bool SortCategoryDatasReverse(Json & InCategoryDataA, Json & InCategoryDataB)
{
	try
	{
		return boost::lexical_cast<double>(std::string(InCategoryDataA["yLabel"])) < boost::lexical_cast<double>(std::string(InCategoryDataB["yLabel"]));
	}
	catch (const std::exception &)
	{
		return false;
	}
}
std::string ConvertEmptyStringToZero(std::string InString)
{
	if (InString == "")
	{
		return "0";
	}
	return InString;
}
std::string ConvertDateToMonth(std::string InDate, char IntervalChar = '.')
{
	std::string Date = FillDate(InDate);
	boost::gregorian::date D_Date = boost::gregorian::from_string(Date);
	return std::to_string(D_Date.year()) + IntervalChar + std::to_string(D_Date.month());
}
std::string ConvertDateToDay(std::string InDate, char IntervalChar = '.')
{
	std::string Date = FillDate(InDate);
	boost::gregorian::date D_Date = boost::gregorian::from_string(Date);
	return std::to_string(D_Date.month()) + IntervalChar + std::to_string(D_Date.day());
}
std::string ConvertDateToMonthOnly(std::string InDate)
{
	std::string Date = FillDate(InDate);
	boost::gregorian::date D_Date = boost::gregorian::from_string(Date);
	return std::to_string(D_Date.month());
}
std::string ConvertToTenThousand(std::string InString, int DecimalPlacesNumber = 2, bool Round = true)
{
	using namespace  boost;
	using namespace  boost::algorithm;

	if (InString == "")
		return "0";
	if (!boost::all(InString, boost::is_digit() || boost::is_any_of("-.")))
		return InString;

	double Number = boost::lexical_cast<double>(InString) / 10000;
	std::string FormatString = "%." + std::to_string(DecimalPlacesNumber) + "f";
	return str(boost::format(FormatString) % (Number));
}
std::string SaveDecimalPlaces(std::string InString, int DecimalPlacesNumber = 2, bool Round = true)
{
	using namespace  boost;
	using namespace  boost::algorithm;

	if (InString == "")
		return "0";
	if (!boost::all(InString, boost::is_digit() || boost::is_any_of("-.")))
		return InString;

	double Number = boost::lexical_cast<double>(InString);
	std::string FormatString = "%." + std::to_string(DecimalPlacesNumber) + "f";
	return str(boost::format(FormatString) % (Number));
}
std::string SavePrecision(std::string InString, int PrecisionNumber = 2, bool Round = true)
{
	using namespace  boost;
	using namespace  boost::algorithm;

	if (InString == "")
		return "0";
	if (!boost::all(InString, boost::is_digit() || boost::is_any_of("-.")))
		return InString;

	double Number = boost::lexical_cast<double>(InString);
	int NumberLength= floor(log10(Number))+1;
	int DecimalPlacesNumber = PrecisionNumber - NumberLength ;
	std::string FormatString = "%." + std::to_string(DecimalPlacesNumber>0? DecimalPlacesNumber:0) + "f";
	return str(boost::format(FormatString) % (Number));
}
std::wstring StringToWString(const std::string & str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	if (len == 0)
	{
		return L"";
	}

	wchar_t * pRes = new wchar_t[len];
	if (pRes == NULL)
	{
		return L"";
	}

	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pRes, len);
	pRes[len - 1] = L'\0';
	std::wstring result = pRes;
	delete[] pRes;

	return result;
}
std::string WStringToString(const std::wstring & wstr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (len == 0)
	{
		return "";
	}

	char * pRes = new char[len];
	if (pRes == NULL)
	{
		return "";
	}

	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pRes, len, NULL, NULL);
	pRes[len - 1] = '\0';
	std::string result = pRes;
	delete[] pRes;

	return result;
}
std::string UTF8ToUnicode(const std::string & strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	if (len == 0)
	{
		return "";
	}

	wchar_t * pRes = new wchar_t[len];
	if (pRes == NULL)
	{
		return "";
	}

	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, pRes, len);
	pRes[len - 1] = L'\0';
	std::wstring result = pRes;
	delete[] pRes;

	return WStringToString(result);
}
std::string UnicodeToUTF8(const std::string & strUnicode)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, StringToWString(strUnicode).c_str(), -1, NULL, 0, NULL, NULL);
	if (len == 0)
	{
		return "";
	}

	char * pRes = new char[len];
	if (pRes == NULL)
	{
		return "";
	}

	WideCharToMultiByte(CP_UTF8, 0, StringToWString(strUnicode).c_str(), -1, pRes, len, NULL, NULL);
	pRes[len - 1] = '\0';
	std::string result = pRes;
	delete[] pRes;

	return result;
}
std::string FillPlaceholderIfNull(std::string InString, int PlaceholderNumber = 5)
{
	try
	{
		if (InString != "")
		{
			if (boost::all(InString, boost::is_digit() || boost::is_any_of("-.")))
			{
				if (boost::lexical_cast<double>(InString) != 0)
				{
					return InString;
				}
			}
			else
			{
				return InString;
			}
		}
		std::string OutString = "";
		for (int i = 0; i < PlaceholderNumber; i++)
		{
			OutString += "-";
		}
		return OutString;

	}
	catch (const std::exception &)
	{
		return InString;

	}
	return InString;

}

std::string GetEnergyConsumptionUnitArea(std::string EnergyConsumption, std::string Area)
{
	if (Area == "" || EnergyConsumption == "")
		return "";
	if (!boost::all(Area, boost::is_digit() || boost::is_any_of(".")))
		return "";

	double EnergyConsumptionUnitArea = boost::lexical_cast<double>(EnergyConsumption) / boost::lexical_cast<double>(Area);
	return SaveDecimalPlaces(std::to_string(EnergyConsumptionUnitArea));
}

OServerHelper & OServerHelper::GetInstance()
{
	static OServerHelper ServerHelper;
	return ServerHelper;
}

Json OServerHelper::BMap(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	Json tableData;
	std::vector<Json> data;

	Json dataJson;
	std::vector<Json> table;
	auto IndustryResults = MaraidbConnector.Query("SELECT IndustryID, IndustryName FROM `BaseIndustryInfo`");
	for (std::vector<std::string> IndustryResult : IndustryResults.DataTable)
	{
		Json tableJson;
		tableJson["name"] = IndustryResult[1];
		tableJson["industryCode"] = IndustryResult[0];
		table.push_back(tableJson);
	}
	dataJson["data"] = table;
	dataJson["title"] = TCHAR_TO_UTF8("行业");
	data.push_back(dataJson);

	std::map<std::string, std::vector<Json>> Types;
	Json DistributionFacilties;

	auto DistributionFaciltiesResults = MaraidbConnector.Query("SELECT CustomerName,CoordinatesPixelX,CoordinatesPixelY,Address,Type,ElecName,StatElecID FROM DistributionFacilties");

	for (std::vector<std::string> DistributionFaciltiesResult : DistributionFaciltiesResults.DataTable)
	{
		std::string Type = DistributionFaciltiesResult[4];
		Json tableJson;
		if (boost::contains(Type, "elecs"))
		{
			tableJson["name"] = DistributionFaciltiesResult[5];
			tableJson["elecStateCode"] = DistributionFaciltiesResult[6];

		}
		else
		{
			tableJson["name"] = DistributionFaciltiesResult[0];
		}
		tableJson["lng"] = DistributionFaciltiesResult[1];
		tableJson["lat"] = DistributionFaciltiesResult[2];
		tableJson["address"] = DistributionFaciltiesResult[3];
		auto TypesIter = Types.find(Type);
		if (TypesIter != Types.end())
		{
			TypesIter->second.push_back(tableJson);
		}
		else
		{
			std::vector<Json> Data;
			Data.push_back(tableJson);
			Types.insert(std::map<std::string, std::vector<Json>>::value_type(Type, Data));
		}
	}
	for (auto TypesIter : Types)
	{
		Json CurDataJson;
		TypesIter.first;
		TypesIter.second;
		CurDataJson["title"] = TypesIter.first;
		CurDataJson["data"] = TypesIter.second;
		data.push_back(CurDataJson);
	}

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	Json EntedataJson;
	std::vector<Json>  EnteTable;

	FMysqlResult EnteInfo = MaraidbConnector.Query(str(boost::format("SELECT BaseEnteInfo.ElecMetID,BaseEnteInfo.EnteName,BaseEnteElecMeterInfo.IndustryID,BaseEnteInfo.Coordinate,BaseEnteInfo.Address,ElecStatEnte.StatElecID,SUM(EnteElecInfo.EnergyConsum) FROM BaseEnteInfo,BaseEnteElecMeterInfo LEFT JOIN EnteElecInfo ON BaseEnteElecMeterInfo.ElecMetID = EnteElecInfo.ElecMetID AND EnteElecInfo.CreateDate BETWEEN '%1%' AND '%2%' LEFT JOIN ElecStatEnte ON BaseEnteElecMeterInfo.ElecMetID=ElecStatEnte.ElecMetID WHERE BaseEnteInfo.ElecMetID = BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('%3%')) GROUP BY BaseEnteElecMeterInfo.ElecMetID;") % StartTime % EndTime % RegionID));
	for (std::vector<std::string> EnteInfoResult : EnteInfo.DataTable)
	{
		std::list<std::string> CoordinatesString;
		boost::split(CoordinatesString, EnteInfoResult[3], boost::is_any_of(","));
		if (CoordinatesString.size() < 2)continue;
		Json tableJson;
		tableJson["elecNum"] = EnteInfoResult[0];
		tableJson["name"] = EnteInfoResult[1];
		tableJson["industryCode"] = EnteInfoResult[2];
		tableJson["lng"] = CoordinatesString.front();
		tableJson["lat"] = CoordinatesString.back();
		tableJson["address"] = EnteInfoResult[4];
		tableJson["elecStateCode"] = EnteInfoResult[5];
		tableJson["energyConsumption"] = EnteInfoResult[6];
		EnteTable.push_back(tableJson);

	}
	EntedataJson["data"] = EnteTable;
	EntedataJson["title"] = TCHAR_TO_UTF8("企业");
	data.push_back(EntedataJson);
	tableData["data"] = data;
	return tableData;

}

Json OServerHelper::CaseInfoBuildTrend(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	FMysqlResult result = MaraidbConnector.Query(str(boost::format("Select month(PublishDate),count(PublishDate) FROM CaseInfo WHERE PublishDate Between '%1%' AND '%2%' GROUP BY month(PublishDate);") % StartTime % EndTime));

	for (int index = 1; index <= 12; index++)
	{
		Json list1;
		Json list2;
		std::string Label;
		std::string YLabel;
		for (std::vector<std::string> resultData : result.DataTable)
		{
			std::string CurrentMonth = std::to_string(index);
			if (CurrentMonth == resultData[0])
			{
				Label = resultData[0];
				YLabel = resultData[1];
				break;
			}
			else
			{
				Label = CurrentMonth;
				YLabel = "0";
			}
		}
		list1["label"] = Label;
		xLabelRang.push_back(list1);
		list2["xLabel"] = Label;
		list2["yLabel"] = YLabel;
		categoryData.push_back(list2);
	}

	data1Json["categoryName"] = TCHAR_TO_UTF8("用能诊断");
	data1Json["start_color"] = "rgba(97,174,255,0.35)";
	data1Json["end_color"] = "rgba(97,174,255,0.35)";
	data1Json["high_color1"] = "#5ceaff";
	data1Json["categoryData"] = categoryData;
	data1.push_back(data1Json);
	tableJson["title"] = TCHAR_TO_UTF8("总");
	tableJson["x_unit"] = TCHAR_TO_UTF8("(月)");
	tableJson["y_unit"] = TCHAR_TO_UTF8("(位)");
	tableJson["data"] = data1;
	tableJson["xLabelRang"] = xLabelRang;
	table.push_back(tableJson);
	dataJson["title"] = TCHAR_TO_UTF8("成功案例建设趋势");
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}

Json OServerHelper::CaseInfoRank(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::map<std::string, std::vector<Json>> Types;
	auto IndustryResults = MaraidbConnector.Query("SELECT CaseImgParh, CaseName, CaseSummary, ViewNum,CaseType FROM `CaseInfo`  ORDER BY CONVERT(ViewNum,INT) DESC");

	for (std::vector<std::string> IndustryResult : IndustryResults.DataTable)
	{
		std::string CurType = IndustryResult[4];
		auto iter = Types.find(CurType);
		Json data1Json;
		data1Json["headUrl"] = IndustryResult[0];
		data1Json["name"] = IndustryResult[1];
		data1Json["remark"] = IndustryResult[2];
		data1Json["num"] = IndustryResult[3];
		data1Json["shool"] = "";
		data1Json["position"] = IndustryResult[4];
		data1Json["phone"] = "";
		data1Json["email"] = "";
		if (iter != Types.end())
		{
			iter->second.push_back(data1Json);
		}
		else
		{
			std::vector<Json> data1;
			data1.push_back(data1Json);
			Types.insert(std::map<std::string, std::vector<Json>>::value_type(CurType, data1));
		}
	}

	std::vector<Json> table;
	for (auto TypesIter : Types)
	{
		Json CurDataJson;
		CurDataJson["type"] = TypesIter.first;
		CurDataJson["data"] = TypesIter.second;
		table.push_back(CurDataJson);
	}
	Json data;
	data["table"] = table;
	data["title"] = TCHAR_TO_UTF8("成功案例排行");
	std::vector<Json> dataArray;
	dataArray.push_back(data);
	Json ReturnData;
	ReturnData["data"] = dataArray;
	return ReturnData;
}

Json OServerHelper::FinanceRank(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::map<std::string, std::vector<Json>> Types;
	auto IndustryResults = MaraidbConnector.Query("SELECT FinanceImgPath, FinanceName, FinanceSummary, ViewNum ,FinacneType FROM `FinanceChannel` ORDER BY CONVERT(ViewNum,INT) DESC");

	for (std::vector<std::string> IndustryResult : IndustryResults.DataTable)
	{
		std::string CurType = IndustryResult[4];
		auto iter = Types.find(CurType);
		Json data1Json;
		data1Json["headUrl"] = IndustryResult[0];
		data1Json["name"] = IndustryResult[1];
		data1Json["remark"] = IndustryResult[2];
		data1Json["num"] = IndustryResult[3];
		data1Json["shool"] = "";
		data1Json["position"] = IndustryResult[4];
		data1Json["phone"] = "";
		data1Json["email"] = "";
		if (iter != Types.end())
		{
			iter->second.push_back(data1Json);
		}
		else
		{
			std::vector<Json> data1;
			data1.push_back(data1Json);
			Types.insert(std::map<std::string, std::vector<Json>>::value_type(CurType, data1));
		}
	}

	std::vector<Json> table;
	for (auto TypesIter : Types)
	{
		Json CurDataJson;
		CurDataJson["type"] = TypesIter.first;
		CurDataJson["data"] = TypesIter.second;
		table.push_back(CurDataJson);
	}
	Json data;
	data["table"] = table;
	data["title"] = TCHAR_TO_UTF8("各类金融类排行");
	std::vector<Json> dataArray;
	dataArray.push_back(data);
	Json ReturnData;
	ReturnData["data"] = dataArray;
	return ReturnData;
}

Json OServerHelper::EnergyClubRank(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::map<std::string, std::vector<Json>> Types;
	auto IndustryResults = MaraidbConnector.Query("SELECT FilePath, ClubName, ClubSummary, ViewNum,ClubType FROM `EnergyClub`  ORDER BY CONVERT(ViewNum,INT) DESC");
	std::vector<Json> All;
	for (std::vector<std::string> IndustryResult : IndustryResults.DataTable)
	{
		std::string CurType = IndustryResult[4];
		auto iter = Types.find(CurType);
		Json data1Json;
		data1Json["headUrl"] = IndustryResult[0];
		data1Json["name"] = IndustryResult[1];
		data1Json["remark"] = IndustryResult[2];
		data1Json["num"] = IndustryResult[3];
		data1Json["shool"] = "";
		data1Json["position"] = IndustryResult[4];
		data1Json["phone"] = "";
		data1Json["email"] = "";
		All.push_back(data1Json);

		if (iter != Types.end())
		{
			iter->second.push_back(data1Json);
		}
		else
		{
			std::vector<Json> data1;
			data1.push_back(data1Json);
			Types.insert(std::map<std::string, std::vector<Json>>::value_type(CurType, data1));
		}
	}

	std::vector<Json> table;
	Json CurDataJson;
	CurDataJson["type"] = TCHAR_TO_UTF8("全部");
	CurDataJson["data"] = All;
	table.push_back(CurDataJson);
	for (auto TypesIter : Types)
	{
		Json CurDataJson;
		CurDataJson["type"] = TypesIter.first;
		CurDataJson["data"] = TypesIter.second;
		table.push_back(CurDataJson);
	}
	Json data;
	data["table"] = table;
	data["title"] = TCHAR_TO_UTF8("能源俱乐部事件排行");
	std::vector<Json> dataArray;
	dataArray.push_back(data);
	Json ReturnData;
	ReturnData["data"] = dataArray;
	return ReturnData;
}

Json OServerHelper::ExpertRank(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::map<std::string, std::vector<Json>> Types;
	auto IndustryResults = MaraidbConnector.Query("SELECT ExpertPhoto, ExpertName, ExpertSummary, ViewNum, ExpertField FROM `ExpertInfo` ORDER BY CONVERT(Sort,INT) ");
	std::vector<Json> All;
	for (std::vector<std::string> IndustryResult : IndustryResults.DataTable)
	{
		std::string CurType = IndustryResult[4];
		auto iter = Types.find(CurType);
		Json data1Json;
		data1Json["headUrl"] = IndustryResult[0];
		data1Json["name"] = IndustryResult[1];
		data1Json["remark"] = IndustryResult[2];
		data1Json["num"] = IndustryResult[3];
		data1Json["shool"] = "";
		data1Json["position"] = IndustryResult[4];
		data1Json["phone"] = "";
		data1Json["email"] = "";
		All.push_back(data1Json);
		if (iter != Types.end())
		{
			iter->second.push_back(data1Json);
		}
		else
		{
			std::vector<Json> data1;
			data1.push_back(data1Json);
			Types.insert(std::map<std::string, std::vector<Json>>::value_type(CurType, data1));
		}
	}

	std::vector<Json> table;
	Json CurDataJson;
	CurDataJson["type"] = TCHAR_TO_UTF8("全部");
	CurDataJson["data"] = All;
	table.push_back(CurDataJson);

	for (auto TypesIter : Types)
	{
		Json CurDataJson;
		CurDataJson["type"] = TypesIter.first;
		CurDataJson["data"] = TypesIter.second;
		table.push_back(CurDataJson);
	}
	Json data;
	data["table"] = table;
	data["title"] = TCHAR_TO_UTF8("专家排行");
	std::vector<Json> dataArray;
	dataArray.push_back(data);
	Json ReturnData;
	ReturnData["data"] = dataArray;
	return ReturnData;
}

Json OServerHelper::FinanceAccess(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	
	FMysqlResult result = MaraidbConnector.Query(str(boost::format("Select month(PublishDate),count(PublishDate) FROM FinanceChannel WHERE PublishDate Between '%1%' AND '%2%' GROUP BY month(PublishDate)") % StartTime % EndTime));
	for (int index = 1; index <= 12; index++)
	{
		Json list1;
		Json list2;
		std::string Label;
		std::string YLabel;
		for (std::vector<std::string> resultData : result.DataTable)
		{
			std::string CurrentMonth = std::to_string(index);
			if (CurrentMonth == resultData[0])
			{
				Label = resultData[0];
				YLabel = resultData[1];
				break;
			}
			else
			{
				Label = CurrentMonth;
				YLabel = "0";
			}
		}
		list1["label"] = Label;
		xLabelRang.push_back(list1);
		list2["xLabel"] = Label;
		list2["yLabel"] = YLabel;
		categoryData.push_back(list2);
	}

	data1Json["categoryName"] = TCHAR_TO_UTF8("用能诊断");//类别一
	data1Json["start_color"] = "rgba(97,174,255,0.35)";
	data1Json["end_color"] = "rgba(97,174,255,0.35)";
	data1Json["high_color1"] = "#5ceaff";
	data1Json["categoryData"] = categoryData;
	data1.push_back(data1Json);
	tableJson["title"] = TCHAR_TO_UTF8("总");//总
	tableJson["x_unit"] = TCHAR_TO_UTF8("(月)");//(万元/tce)
	tableJson["y_unit"] = TCHAR_TO_UTF8("(个)");
	tableJson["data"] = data1;
	tableJson["xLabelRang"] = xLabelRang;
	table.push_back(tableJson);
	dataJson["title"] = TCHAR_TO_UTF8("金融产品接入趋势");//各行业万元产值能耗
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}

Json OServerHelper::FinanceType(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	auto result = MaraidbConnector.Query("Select FinacneType,COUNT(FinacneType) FROM FinanceChannel GROUP BY FinacneType;");
	for (std::vector<std::string> resultData : result.DataTable)
	{
		tableJson["title"] = resultData[0];
		tableJson["xLabel"] = "";
		tableJson["yLabel"] = resultData[1]+TCHAR_TO_UTF8("款");
		table.push_back(tableJson);
	}
	dataJson["table"] = table;
	dataJson["title"] = TCHAR_TO_UTF8("金融渠道类型分布");
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}

Json OServerHelper::ExpertField(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	auto result = MaraidbConnector.Query("Select ExpertField FROM ExpertInfo;");
	struct FExpertFieldType
	{
		std::string FieldName;
		int FieldNum = 0;
		static void split(const std::string & s, std::vector<std::string> & sv, const char flag = ' ')
		{
			sv.clear();
			std::istringstream iss(s);
			std::string temp;

			while (std::getline(iss, temp, flag))
			{
				sv.push_back(std::string(temp));
			}
			return;
		}
	};
	std::vector<FExpertFieldType> ExpertFieldTypes;
	for (std::vector<std::string> resultData : result.DataTable)
	{

		std::vector<std::string> FieldNames;
		if (resultData[0] != "")
		{
			FExpertFieldType::split(resultData[0], FieldNames, ',');
			for (std::string FieldName : FieldNames)
			{
				bool IsAddField = true;

				for (FExpertFieldType & CurrentFieldName : ExpertFieldTypes)
				{
					if (CurrentFieldName.FieldName == FieldName)
					{
						CurrentFieldName.FieldNum++;
						IsAddField = false;
						break;
					}
					else
					{
						IsAddField = true;
					}
				}
				if (IsAddField)
				{
					FExpertFieldType CurExpertFieldType;
					CurExpertFieldType.FieldName = FieldName;
					CurExpertFieldType.FieldNum++;
					ExpertFieldTypes.push_back(CurExpertFieldType);
				}
			}
		}

	}
	for (FExpertFieldType CurrentField : ExpertFieldTypes)
	{
		Json list1;
		Json list2;
		std::string Label;
		std::string YLabel;
		Label = CurrentField.FieldName;
		YLabel = std::to_string(CurrentField.FieldNum);
		list1["label"] = Label;
		xLabelRang.push_back(list1);
		list2["xLabel"] = Label;
		list2["yLabel"] = YLabel;
		categoryData.push_back(list2);

	}
	data1Json["categoryName"] = TCHAR_TO_UTF8("类别一");
	data1Json["start_color"] = "rgba(255,255,255,0.31)";
	data1Json["end_color"] = "rgba(255,255,255,0.74)";
	data1Json["high_color1"] = "#F2F2F2";
	data1Json["high_color2"] = "#fce700";
	data1Json["categoryData"] = categoryData;
	data1.push_back(data1Json);
	tableJson["title"] = TCHAR_TO_UTF8("总");
	tableJson["unit"] = TCHAR_TO_UTF8("位");
	tableJson["data"] = data1;
	tableJson["xLabelRang"] = xLabelRang;
	table.push_back(tableJson);
	dataJson["title"] = TCHAR_TO_UTF8("领域入驻专家");
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}

Json OServerHelper::EnergyClubType(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	auto ClubResult = MaraidbConnector.Query("Select ClubType,COUNT(ClubType) FROM EnergyClub GROUP BY ClubType");

	std::string Keys[5] = { TCHAR_TO_UTF8("会展"),TCHAR_TO_UTF8("会议"),TCHAR_TO_UTF8("活动"),TCHAR_TO_UTF8("培训"),TCHAR_TO_UTF8("沙龙") };

	std::vector<Json> XLabelRang;
	for (auto Key:Keys)
	{
		Json Label;
		Label["label"] = Key;
		XLabelRang.push_back(Label);
	}

	std::vector<Json> CategoryDatas;

	for (auto ClubResultRow:ClubResult.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = ClubResultRow[0];
		CategoryData["yLabel"] = ClubResultRow[1];
		CategoryDatas.push_back(CategoryData);
	}
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.4)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = FillCategoryData(XLabelRang,CategoryDatas);

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);
	
	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = XLabelRang;
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("能源俱乐部类型统计");
	Data["table"] = Tables;
	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::CaseInfoProjectType(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	auto result = MaraidbConnector.Query("Select CaseType,Count(CaseType) from CaseInfo GROUP BY CaseType");
	for (std::vector<std::string> resultData : result.DataTable)
	{
		Json list1;
		Json list2;
		std::string Label;
		std::string YLabel;
		if (resultData[0] == TCHAR_TO_UTF8("优秀成果") || resultData[0] == TCHAR_TO_UTF8("孵化项目"))
		{
			Label = resultData[0];
			YLabel = resultData[1];
			list1["label"] = Label;
			xLabelRang.push_back(list1);
			list2["xLabel"] = Label;
			list2["yLabel"] = YLabel;
			categoryData.push_back(list2);
		}
	}
	data1Json["categoryName"] = TCHAR_TO_UTF8("类别一");
	data1Json["start_color"] = "rgba(255,255,255,0.31)";
	data1Json["end_color"] = "rgba(255,255,255,0.74)";
	data1Json["high_color1"] = "#F2F2F2";
	data1Json["high_color2"] = "#fce700";
	data1Json["categoryData"] = categoryData;
	data1.push_back(data1Json);
	tableJson["title"] = TCHAR_TO_UTF8("总");
	tableJson["unit"] = TCHAR_TO_UTF8("位");
	tableJson["data"] = data1;
	tableJson["xLabelRang"] = xLabelRang;
	table.push_back(tableJson);
	dataJson["title"] = TCHAR_TO_UTF8("优秀成果及孵化项目");
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}

Json OServerHelper::ExpertTrend(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	FMysqlResult result = MaraidbConnector.Query(str(boost::format("Select month(CreateDate),count(CreateDate) FROM ExpertInfo WHERE CreateDate Between '%1%' AND '%2%' GROUP BY month(CreateDate)") % StartTime % EndTime));

	for (int index = 1; index <= 12; index++)
	{
		Json list1;
		Json list2;
		std::string Label;
		std::string YLabel;
		for (std::vector<std::string> resultData : result.DataTable)
		{
			std::string CurrentMonth = std::to_string(index);
			if (CurrentMonth == resultData[0])
			{
				Label = resultData[0];
				YLabel = resultData[1];
				break;
			}
			else
			{
				Label = CurrentMonth;
				YLabel = "0";
			}
		}
		list1["label"] = Label;
		xLabelRang.push_back(list1);
		list2["xLabel"] = Label;
		list2["yLabel"] = YLabel;
		categoryData.push_back(list2);
	}

	data1Json["categoryName"] = TCHAR_TO_UTF8("用能诊断");
	data1Json["start_color"] = "rgba(97,174,255,0.35)";
	data1Json["end_color"] = "rgba(97,174,255,0.35)";
	data1Json["high_color1"] = "#5ceaff";
	data1Json["categoryData"] = categoryData;
	data1.push_back(data1Json);
	tableJson["title"] = TCHAR_TO_UTF8("总");
	tableJson["unit"] = TCHAR_TO_UTF8("月");
	tableJson["data"] = data1;
	tableJson["xLabelRang"] = xLabelRang;
	table.push_back(tableJson);
	dataJson["title"] = TCHAR_TO_UTF8("专家入驻趋势");
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}

Json OServerHelper::NewEventList(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	
	FMysqlResult Result = MaraidbConnector.Query(str(boost::format("SELECT ClubName,ClubType,ClubDate FROM EnergyClub ORDER BY EnergyClub.ClubDate DESC;")));

	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("活动名称") ,TCHAR_TO_UTF8("活动类型"),TCHAR_TO_UTF8("举办时间") };
	std::vector<std::string > RowWidth = { "18.27" ,"14.27" ,"24.27" };

	Json Table = FillTableJson(RowHeader, RowWidth, Result.DataTable, 6);

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"]= TCHAR_TO_UTF8("最新事件列表");
	Data["table"]= Tables;
	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}


Json OServerHelper::ListOfLabelCompanies(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string LabelID = FIELDIFNULL(InData["LabelID"]);

	Json TableData;
	std::vector<Json> Data;

	if (RegionID != "" && LabelID != "")
	{
		FMysqlResult Result = MaraidbConnector.Query("SELECT BaseEnteInfo.EnteID,BaseEnteInfo.EnteName,BaseIndustryInfo.IndustryName FROM BaseEnteInfo,EnteLabel,BaseRegionInfo,BaseEnteElecMeterInfo Left Join BaseIndustryInfo On BaseEnteElecMeterInfo.IndustryID=BaseIndustryInfo.IndustryID WHERE FIND_IN_SET(BaseRegionInfo.RegionID, GetChildrenRegion('" + RegionID + "')) AND BaseEnteInfo.RegionID = BaseRegionInfo.RegionID AND BaseEnteInfo.EnteID = EnteLabel.EnteID AND EnteLabel.LabelName!='' AND !IsNULL(EnteLabel.LabelName)  AND BaseEnteElecMeterInfo.ElecMetID=BaseEnteInfo.ElecMetID AND EnteLabel.LabelID='" + LabelID + "'");
		for (std::vector<std::string> ResultData : Result.DataTable)
		{
			Json List;
			List["elecNum"] = ResultData[0];
			List["enteName"] = ResultData[1];
			List["industryName"] = ResultData[2];
			Data.push_back(List);
		}
	}

	TableData["title"] = TCHAR_TO_UTF8("标签企业列表");
	TableData["data"] = Data;
	return TableData;
}

Json OServerHelper::LabelCallRanking(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string Month = FIELDIFNULL(InData["Month"]);

	std::string RequestAPI = HTTP_URL("/sts/out/label/res/top");
	std::string RequsetData = "?month=" + Month;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Datas;
	for (auto DataRow: HttpReceiveInfo["data"])
	{
		std::string GroupName = DataRow["resName"];
		FMysqlResult SubLabelInfo = MaraidbConnector.Query(str(boost::format("SELECT LabelName,LabelID FROM LabelInfo,LabelGroupInfo WHERE LabelInfo.GroupID=LabelGroupInfo.GroupID AND LabelGroupInfo.GroupName='%1%';") % GroupName));

		std::vector<Json> SubLabels;
		for (auto SubLabelInfoRow: SubLabelInfo.DataTable)
		{
			Json SubLabel;
			SubLabel["labelName"] = SubLabelInfoRow[0];
			SubLabel["labelID"] = SubLabelInfoRow[1];

			SubLabels.push_back(SubLabel);
		}
		DataRow["subLabel"] = SubLabels;
		Datas.push_back(DataRow);
	}
	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::EnterpriseLabelEvolutionTrend(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult LabelEvolutionResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(SnapshotDate),MONTH(SnapshotDate),COUNT(LabelCode) FROM EnteLabelSnapshot WHERE ElecMetID='%1%' AND SnapshotDate BETWEEN '%2%' AND '%3%' GROUP BY SnapshotDate ORDER BY SnapshotDate;") % ElecMetID % StartTime % EndTime));

	FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, Config);

	std::vector<Json> CategoryDatas;
	for (auto LabelEvolutionResultRow : LabelEvolutionResult.DataTable)
		CategoryDatas.push_back({ {"xLabel",LabelEvolutionResultRow[0] + "." + LabelEvolutionResultRow[1]},
								 {"yLabel",LabelEvolutionResultRow[2]}
			});

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0)";
	TableData["end_color"] = "rgba(209,246,255,0.3)";
	TableData["high_color1"] = "#F2F2F2";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("企业标签演变趋势");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::EnterpriseProfile(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);

	std::string EnteInfoCommand = "SELECT BaseEnteInfo.EnteName,IFNULL(BaseIndustryInfo.IndustryName,''),BaseEnteInfo.VoltageLevel,BaseEnteInfo.JoinDate,BaseRegionInfo.RegionName,BaseEnteInfo.RunCapacity,BaseEnteInfo.Address FROM BaseEnteInfo,BaseRegionInfo,BaseEnteElecMeterInfo LEFT JOIN BaseIndustryInfo ON BaseEnteElecMeterInfo.IndustryID=BaseIndustryInfo.IndustryID WHERE BaseEnteElecMeterInfo.ElecMetID=BaseEnteInfo.ElecMetID AND BaseRegionInfo.RegionID=BaseEnteInfo.RegionID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';";
	auto EnteInfo = MaraidbConnector.Query(EnteInfoCommand);

	std::string EnteLabelListCommand = "SELECT DISTINCT EnteLabel.LabelName FROM EnteLabel WHERE EnteLabel.ElecMetID='" + ElecMetID + "'";
	auto EnteLabelList = MaraidbConnector.Query(EnteLabelListCommand);

	std::vector<Json> EnteLabels;
	for (auto EnteLabelListRow : EnteLabelList.DataTable)
	{
		Json EnteLabel;
		EnteLabel["value"] = EnteLabelListRow[0];
		EnteLabels.push_back(EnteLabel);
	}

	if (EnteInfo.ResultIsNotNull())
	{
		Json Data;
		Data[TCHAR_TO_UTF8("行业类型")] = EnteInfo.DataTable[0][1];
		Data[TCHAR_TO_UTF8("电压等级")] = EnteInfo.DataTable[0][2];
		Data[TCHAR_TO_UTF8("入驻日期")] = FillPlaceholderIfNull(EnteInfo.DataTable[0][3],6);
		Data[TCHAR_TO_UTF8("所属区域")] = EnteInfo.DataTable[0][4];
		Data[TCHAR_TO_UTF8("运行容量")] = EnteInfo.DataTable[0][5] + "kVA";
		Data[TCHAR_TO_UTF8("用电地址")] = EnteInfo.DataTable[0][6];
		Data[TCHAR_TO_UTF8("标签")] = EnteLabels;

		Json ReturnData;
		ReturnData["title"] = EnteInfo.DataTable[0][0];
		ReturnData["data"] = Data;

		return ReturnData;
	}
	else
	{
		Json Data;
		Data[TCHAR_TO_UTF8("行业类型")] = "----";
		Data[TCHAR_TO_UTF8("电压等级")] = "----";
		Data[TCHAR_TO_UTF8("入驻日期")] = "----";
		Data[TCHAR_TO_UTF8("所属区域")] = "----";
		Data[TCHAR_TO_UTF8("运行容量")] = "----";
		Data[TCHAR_TO_UTF8("用电地址")] = "----";
		Data[TCHAR_TO_UTF8("标签")] = EnteLabels;

		Json ReturnData;
		ReturnData["title"] = TCHAR_TO_UTF8("----");
		ReturnData["data"] = Data;

		return ReturnData;

	}
}

//5.10
Json OServerHelper::CustomerFeatureAnalysisMap(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string LabelID = FIELDIFNULL(InData["LabelID"]);
	std::vector<Json> Data;
	Json TableData;
	if (RegionID != "" && LabelID != "") {
		auto result = MaraidbConnector.Query("Call GetRegionLabelCount('" + RegionID + "','" + LabelID + "')");
		for (std::vector<std::string> resultData : result.DataTable) {
			Json RegionData;
			RegionData["regionName"] = resultData[1];
			RegionData["regionId"] = resultData[0];
			RegionData["labelCount"] = resultData[2];
			Data.push_back(RegionData);
		}
		TableData["data"] = Data;
		return TableData;
	}
	return {};
}//
//5.11

//拿不到数据时间
Json OServerHelper::ProportionOfIndustrialEnergyConsumption(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string RegionID = boost::lexical_cast<std::string>(FIELDIFNULL(InData["regionID"]));
	Json tableData;
	std::vector<Json>data1;
	Json data1Json;
	std::vector<Json>table;
	Json tableJson;
	std::vector<Json>data2;
	Json data2Json;
	std::vector<Json> categoryData;
	auto result = MaraidbConnector.Query("Call GetRegionInfo_IndustryName_Num(" + RegionID + ")");
	for (std::vector<std::string>resultData : result.DataTable) {
		Json list;
		list["xLabel"] = resultData[0];
		list["yLabel"] = resultData[1];
		categoryData.push_back(list);
	}
	data2Json["categoryName"] = TCHAR_TO_UTF8("需");//需
	data2Json["start_color"] = "#8FC31F";
	data2Json["end_color"] = "#8FC31F";
	data2Json["high_color"] = "#5ceaff";
	data2Json["gradient_color"] = "#8FC31F5A";
	data2Json["categoryData"] = categoryData;
	data2.push_back(data2Json);
	tableJson["unit"] = TCHAR_TO_UTF8("万tce");//万tce
	tableJson["data"] = data2;
	tableJson["title"] = TCHAR_TO_UTF8("总");//总
	table.push_back(tableJson);
	data1Json["title"] = TCHAR_TO_UTF8("产业能源消费占比");//产业能源消费占比
	data1Json["table"] = table;
	data1.push_back(data1Json);
	tableData["data"] = data1;
	return tableData;
}

Json OServerHelper::EnergyEfficiencyInVariousIndustries(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	FMysqlResult RegionNameResult = MaraidbConnector.Query(str(boost::format("SELECT RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.RegionID='%1%';") % RegionID));



	std::string Title[3] = { TCHAR_TO_UTF8("单位面积能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("度电经济增加值") };
	std::string Unit[3] = { TCHAR_TO_UTF8("tce/km²"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("万元/kW·h") };

	FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false,
		'.', EXLabelStepDateIterator::Month);

	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, Config);
	std::vector<Json> Tables;

	for (int Index=0;Index<3;Index++)
	{
		std::vector<Json> EnergyEfficiencyCategoryDatas;

		for (auto ChildIndustryListRow : ChildIndustryList.DataTable)
		{
			std::string IndustryEnergyEfficiencyCommand = "SELECT SUM(EnergyConsumUnitArea),SUM(OutputValue),AVG(EnergyEcnoValueAdd) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + RegionID + "')) AND IndustryID='" + ChildIndustryListRow[0] + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
			FMysqlResult IndustryEnergyEfficiency = MaraidbConnector.Query(IndustryEnergyEfficiencyCommand);

			Json EnergyEfficiencyCategoryData;
			EnergyEfficiencyCategoryData["xLabel"] = ChildIndustryListRow[1];
			EnergyEfficiencyCategoryData["yLabel"] = SaveDecimalPlaces(IndustryEnergyEfficiency.DataTable[0][Index]);
			EnergyEfficiencyCategoryDatas.push_back(EnergyEfficiencyCategoryData);
		}
		std::sort(EnergyEfficiencyCategoryDatas.begin(), EnergyEfficiencyCategoryDatas.end(), SortCategoryDatas);

		Json EnergyEfficiencyTableData;
		EnergyEfficiencyTableData["categoryName"] = "";
		EnergyEfficiencyTableData["categoryData"] = EnergyEfficiencyCategoryDatas;

		std::vector<Json>EnergyEfficiencyTableDatas;
		EnergyEfficiencyTableDatas.push_back(EnergyEfficiencyTableData);

		Json EnergyEfficiencyTable;
		EnergyEfficiencyTable["title"] = Title[Index];
		EnergyEfficiencyTable["unit"] = Unit[Index];
		EnergyEfficiencyTable["data"] = EnergyEfficiencyTableDatas;

		Tables.push_back(EnergyEfficiencyTable);
	}

	Json Data;
	std::vector<Json> Datas;

	if (RegionNameResult.ResultIsNotNull())
	{
		Data["title"] = TCHAR_TO_UTF8("各行业能效")+ RegionNameResult.DataTable[0][0];
	}
	else
	{
		Data["title"] = TCHAR_TO_UTF8("各行业能效");
	}
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;


}

Json OServerHelper::EnergyEffiencyTrendsInThePastYear(Json InData)
{
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(Area) FROM BaseEnteInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('%1%'));") % RegionID));
	double Area = lexical_cast<double>(RegionArea.DataTable[0][0]) / 1000000;

	FMysqlResult RegionEnergyEffiencyResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),EnergyConsumUnitArea,OutputValue,EnergyEcnoValueAdd,SynEnergyConsum FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % RegionID % StartTime % EndTime));
	std::string Title[3] = { FIELDIFNULL("单位面积能耗"),FIELDIFNULL("万元产值能耗"),FIELDIFNULL("度电经济增加值") };
	std::string Unit[3] = { FIELDIFNULL("tce/km²"),FIELDIFNULL("tce/万元"),FIELDIFNULL("万元/kW·h") };

	FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false,
		'.', EXLabelStepDateIterator::Month);

	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, Config);
	std::vector<Json> ChartDatas;
	for (int Index = 0; Index < 3; Index++)
	{
		std::vector<Json> CategoryDatas;
		for (auto RegionEnergyEffiencyResultRow : RegionEnergyEffiencyResult.DataTable)
		{
			if (Index == 0)
			{
				double SynEnergyConsum = lexical_cast<double>(RegionEnergyEffiencyResultRow[5]);
				double EnergyConsumUnitArea = SynEnergyConsum / Area;
				CategoryDatas.push_back({ {"xLabel",RegionEnergyEffiencyResultRow[0] + "." + RegionEnergyEffiencyResultRow[1]},
					 {"yLabel",std::to_string(EnergyConsumUnitArea)}
					});
			}
			else
			{
				CategoryDatas.push_back({ {"xLabel",RegionEnergyEffiencyResultRow[0] + "." + RegionEnergyEffiencyResultRow[1]},
					 {"yLabel",RegionEnergyEffiencyResultRow[2 + Index]}
					});
			}

		}

		Json TableData;
		TableData["start_color"] = "rgba(97,174,255,0.1)";
		TableData["end_color"] = "rgba(97,174,255,0.35)";
		TableData["high_color"] = "#5ceaff";
		TableData["gradient_color"] = "#8FC31F";
		TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

		std::vector<Json> TableDatas;
		TableDatas.push_back(TableData);

		Json Table;
		Table["unit"] = Unit[Index];
		Table["xLabelRang"] = XLabelRangs;
		Table["data"] = TableDatas;

		std::vector<Json> Tables;
		Tables.push_back(Table);

		Json ChartData;
		ChartData["title"] = Title[Index];
		ChartData["data"] = Tables;

		ChartDatas.push_back(ChartData);
	}
	Json ReturnData;
	ReturnData["chartName"] = TCHAR_TO_UTF8("近一年能效趋势");
	ReturnData["chartData"] = ChartDatas;
	return ReturnData;
}

//设备信息没有数据
Json OServerHelper::RegionalEnergyContrast(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	return ""_json;

}

//区域信息没有数据
Json OServerHelper::MonthlyRegionalEnergyUseAnalysis(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();



	return {};
}

//接口还未测试
Json OServerHelper::EnergyConsumptionByRegion(Json InData) {
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> OutputValueCategoryDatas;
	std::vector<Json> EnergyConsumUnitAreaCategoryDatas;
	std::vector<Json> EnergyEcnoValueAddCategoryDatas;

	std::vector<Json> XLabelRangs;
	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		Json XLabelRang;
		XLabelRang["label"] = ChildRegion[1];
		XLabelRangs.push_back(XLabelRang);

		//0: 万元产值综合能耗  1：单位建筑面积能耗  2：度电经济增加值 3.综合能耗
		std::string RegionEnergyEfficiencyCommand = "CALL GetRegionEnergyEfficiency('" + ChildRegion[0] + "', '" + StartTime + "', '" + EndTime + "');";
		FMysqlResult RegionEnergyEfficiency = MaraidbConnector.Query(RegionEnergyEfficiencyCommand);

		Json OutputValueCategoryData;
		OutputValueCategoryData["xLabel"] = ChildRegion[1];
		OutputValueCategoryData["yLabel"] = RegionEnergyEfficiency.DataTable[0][0];
		OutputValueCategoryDatas.push_back(OutputValueCategoryData);

		Json EnergyConsumUnitAreaCategoryData;
		EnergyConsumUnitAreaCategoryData["xLabel"] = ChildRegion[1];
		EnergyConsumUnitAreaCategoryData["yLabel"] = SaveDecimalPlaces(RegionEnergyEfficiency.DataTable[0][1]);
		EnergyConsumUnitAreaCategoryDatas.push_back(EnergyConsumUnitAreaCategoryData);


		Json EnergyEcnoValueAddCategoryData;
		EnergyEcnoValueAddCategoryData["xLabel"] = ChildRegion[1];
		EnergyEcnoValueAddCategoryData["yLabel"] = RegionEnergyEfficiency.DataTable[0][2];
		EnergyEcnoValueAddCategoryDatas.push_back(EnergyEcnoValueAddCategoryData);
	}

	Json OutputValueTableData;
	OutputValueTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	OutputValueTableData["start_color"] = "rgba(209,246,255,1)";
	OutputValueTableData["end_color"] = "rgba(209,246,255,1)";
	OutputValueTableData["high_color"] = "#5ceaff";
	OutputValueTableData["categoryData"] = OutputValueCategoryDatas;

	std::sort(EnergyConsumUnitAreaCategoryDatas.begin(), EnergyConsumUnitAreaCategoryDatas.end(), SortCategoryDatasReverse);
	Json EnergyConsumUnitAreaTableData;
	EnergyConsumUnitAreaTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnergyConsumUnitAreaTableData["start_color"] = "rgba(209,246,255,1)";
	EnergyConsumUnitAreaTableData["end_color"] = "rgba(209,246,255,1)";
	EnergyConsumUnitAreaTableData["high_color"] = "#5ceaff";
	EnergyConsumUnitAreaTableData["categoryData"] = EnergyConsumUnitAreaCategoryDatas;

	Json EnergyEcnoValueAddTableData;
	EnergyEcnoValueAddTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnergyEcnoValueAddTableData["start_color"] = "rgba(209,246,255,1)";
	EnergyEcnoValueAddTableData["end_color"] = "rgba(209,246,255,1)";
	EnergyEcnoValueAddTableData["high_color"] = "#5ceaff";
	EnergyEcnoValueAddTableData["categoryData"] = EnergyEcnoValueAddCategoryDatas;

	std::vector<Json>OutputValueTableDatas;
	std::vector<Json>EnergyConsumUnitAreaTableDatas;
	std::vector<Json>EnergyEcnoValueAddTableDatas;

	OutputValueTableDatas.push_back(OutputValueTableData);
	EnergyConsumUnitAreaTableDatas.push_back(EnergyConsumUnitAreaTableData);
	EnergyEcnoValueAddTableDatas.push_back(EnergyEcnoValueAddTableData);

	Json OutputValueTable;
	OutputValueTable["title"] = TCHAR_TO_UTF8("万元产值综合能耗");
	OutputValueTable["unit"] = TCHAR_TO_UTF8("tce/万元");
	OutputValueTable["xLabelRang"] = XLabelRangs;
	OutputValueTable["data"] = OutputValueTableDatas;

	Json EnergyConsumUnitAreaTable;
	EnergyConsumUnitAreaTable["title"] = TCHAR_TO_UTF8("单位建筑面积能耗");
	EnergyConsumUnitAreaTable["unit"] = TCHAR_TO_UTF8("tce/km²");
	EnergyConsumUnitAreaTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnergyConsumUnitAreaCategoryDatas);
	EnergyConsumUnitAreaTable["data"] = EnergyConsumUnitAreaTableDatas;

	Json EnergyEcnoValueAddTable;
	EnergyEcnoValueAddTable["title"] = TCHAR_TO_UTF8("度电经济增加值");
	EnergyEcnoValueAddTable["unit"] = TCHAR_TO_UTF8("元/kW·h");
	EnergyEcnoValueAddTable["xLabelRang"] = XLabelRangs;
	EnergyEcnoValueAddTable["data"] = EnergyEcnoValueAddTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnergyConsumUnitAreaTable);
	Tables.push_back(OutputValueTable);
	Tables.push_back(EnergyEcnoValueAddTable);


	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("各区能效");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::CarbonEmissionsByRegion(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> CarbonEmissionCategoryDatas;

	std::vector<Json> XLabelRangs;
	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionCarbonEmissionCommand = "CALL GetRegionCarbonEmission('" + ChildRegion[0] + "', '" + StartTime + "', '" + EndTime + "');";
		FMysqlResult RegionCarbonEmission = MaraidbConnector.Query(RegionCarbonEmissionCommand);

		Json CarbonEmissionCategoryData;
		CarbonEmissionCategoryData["xLabel"] = ChildRegion[1];
		CarbonEmissionCategoryData["yLabel"] = ConvertToTenThousand(RegionCarbonEmission.DataTable[0][0]);
		CarbonEmissionCategoryDatas.push_back(CarbonEmissionCategoryData);
	}
	std::sort(CarbonEmissionCategoryDatas.begin(), CarbonEmissionCategoryDatas.end(), SortCategoryDatasReverse);

	Json CarbonEmissionTableData;
	CarbonEmissionTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	CarbonEmissionTableData["start_color"] = "rgba(209,246,255,1)";
	CarbonEmissionTableData["end_color"] = "rgba(209,246,255,1)";
	CarbonEmissionTableData["high_color"] = "#5ceaff";
	CarbonEmissionTableData["categoryData"] = CarbonEmissionCategoryDatas;



	std::vector<Json>CarbonEmissionTableDatas;
	CarbonEmissionTableDatas.push_back(CarbonEmissionTableData);

	Json CarbonEmissionTable;
	CarbonEmissionTable["title"] = TCHAR_TO_UTF8("总");
	CarbonEmissionTable["unit"] = TCHAR_TO_UTF8("万t");
	CarbonEmissionTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(CarbonEmissionCategoryDatas);
	CarbonEmissionTable["data"] = CarbonEmissionTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(CarbonEmissionTable);



	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("各区域碳排放");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ComprehensiveEnergyConsumptionByRegion(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> EnergyConsumptionCategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionEnergyConsumptionCommand = "CALL GetRegionEnergyConsumption('" + ChildRegion[0] + "', '" + StartTime + "', '" + EndTime + "');";
		FMysqlResult RegionEnergyConsumption = MaraidbConnector.Query(RegionEnergyConsumptionCommand);

		Json EnergyConsumptionCategoryData;
		EnergyConsumptionCategoryData["xLabel"] = ChildRegion[1];
		EnergyConsumptionCategoryData["yLabel"] = ConvertToTenThousand(RegionEnergyConsumption.DataTable[0][0]);
		EnergyConsumptionCategoryDatas.push_back(EnergyConsumptionCategoryData);
	}
	std::sort(EnergyConsumptionCategoryDatas.begin(), EnergyConsumptionCategoryDatas.end(), SortCategoryDatasReverse);

	Json EnergyConsumptionTableData;
	EnergyConsumptionTableData["categoryName"] = FIELDIFNULL("类别一");
	EnergyConsumptionTableData["start_color"] = "rgba(209,246,255,1)";
	EnergyConsumptionTableData["end_color"] = "rgba(209,246,255,1)";
	EnergyConsumptionTableData["high_color"] = "#5ceaff";
	EnergyConsumptionTableData["categoryData"] = EnergyConsumptionCategoryDatas;

	std::vector<Json>EnergyConsumptionTableDatas;
	EnergyConsumptionTableDatas.push_back(EnergyConsumptionTableData);

	Json EnergyConsumptionTable;
	EnergyConsumptionTable["title"] = FIELDIFNULL("总");
	EnergyConsumptionTable["unit"] = FIELDIFNULL("万tce");
	EnergyConsumptionTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnergyConsumptionCategoryDatas);
	EnergyConsumptionTable["data"] = EnergyConsumptionTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnergyConsumptionTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("各区域综合能耗");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ComprehensiveEnergyConsumptionByIndustry(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionNameResult = MaraidbConnector.Query(str(boost::format("SELECT RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.RegionID='%1%';") % RegionID));

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> EnergyConsumptionCategoryDatas;

	for (auto ChildIndustryListRow : ChildIndustryList.DataTable)
	{
		std::string IndustryEnergyConsumptionCommand = "SELECT SUM(SynEnergyConsum) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + RegionID + "')) AND IndustryID='" + ChildIndustryListRow[0] + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
		FMysqlResult IndustryEnergyConsumption = MaraidbConnector.Query(IndustryEnergyConsumptionCommand);

		Json EnergyConsumptionCategoryData;
		EnergyConsumptionCategoryData["xLabel"] = ChildIndustryListRow[1];
		EnergyConsumptionCategoryData["yLabel"] = SaveDecimalPlaces(IndustryEnergyConsumption.DataTable[0][0]);
		EnergyConsumptionCategoryDatas.push_back(EnergyConsumptionCategoryData);
	}
	std::sort(EnergyConsumptionCategoryDatas.begin(), EnergyConsumptionCategoryDatas.end(), SortCategoryDatas);

	Json EnergyConsumptionTableData;
	EnergyConsumptionTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnergyConsumptionTableData["start_color"] = "rgba(45,253,255,0.25)";
	EnergyConsumptionTableData["end_color"] = "rgba(45,253,255,1)";
	EnergyConsumptionTableData["high_color1"] = "#F2F2F2";
	EnergyConsumptionTableData["high_color2"] = "#fce700";
	EnergyConsumptionTableData["categoryData"] = EnergyConsumptionCategoryDatas;

	std::vector<Json>EnergyConsumptionTableDatas;
	EnergyConsumptionTableDatas.push_back(EnergyConsumptionTableData);

	Json EnergyConsumptionTable;
	EnergyConsumptionTable["title"] = TCHAR_TO_UTF8("总");
	EnergyConsumptionTable["unit"] = "(tce)";
	EnergyConsumptionTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnergyConsumptionCategoryDatas);
	EnergyConsumptionTable["data"] = EnergyConsumptionTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnergyConsumptionTable);

	Json Data;
	std::vector<Json> Datas;

	if (RegionNameResult.ResultIsNotNull())
	{
		Data["title"] = TCHAR_TO_UTF8("各行业综合能耗-") + RegionNameResult.DataTable[0][0];
	}
	else
	{
		Data["title"] = TCHAR_TO_UTF8("各行业综合能耗");

	}
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::IndustryEnergyConsumptionRatio(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionNameResult = MaraidbConnector.Query(str(boost::format("SELECT RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.RegionID='%1%';") % RegionID));

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> EnergyConsumptionCategoryDatas;

	for (auto ChildIndustryListRow : ChildIndustryList.DataTable)
	{
		std::string IndustryEnergyConsumptionCommand = "SELECT SUM(SynEnergyConsum) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + RegionID + "')) AND IndustryID='" + ChildIndustryListRow[0] + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
		FMysqlResult IndustryEnergyConsumption = MaraidbConnector.Query(IndustryEnergyConsumptionCommand);

		Json EnergyConsumptionCategoryData;
		EnergyConsumptionCategoryData["xLabel"] = ChildIndustryListRow[1];
		EnergyConsumptionCategoryData["yLabel"] = SaveDecimalPlaces(IndustryEnergyConsumption.DataTable[0][0]);
		EnergyConsumptionCategoryDatas.push_back(EnergyConsumptionCategoryData);
	}
	std::sort(EnergyConsumptionCategoryDatas.begin(), EnergyConsumptionCategoryDatas.end(), SortCategoryDatas);
	EnergyConsumptionCategoryDatas = FiltrationSmallDataByRatio(EnergyConsumptionCategoryDatas, 0.02f);

	Json EnergyConsumptionTableData;
	EnergyConsumptionTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnergyConsumptionTableData["start_color"] = "rgba(45,253,255,0.25)";
	EnergyConsumptionTableData["end_color"] = "rgba(45,253,255,1)";
	EnergyConsumptionTableData["high_color1"] = "#F2F2F2";
	EnergyConsumptionTableData["high_color2"] = "#fce700";
	EnergyConsumptionTableData["categoryData"] = EnergyConsumptionCategoryDatas;

	std::vector<Json>EnergyConsumptionTableDatas;
	EnergyConsumptionTableDatas.push_back(EnergyConsumptionTableData);

	Json EnergyConsumptionTable;
	EnergyConsumptionTable["title"] = TCHAR_TO_UTF8("总");
	EnergyConsumptionTable["unit"] = "(tce)";
	EnergyConsumptionTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnergyConsumptionCategoryDatas);
	EnergyConsumptionTable["data"] = EnergyConsumptionTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnergyConsumptionTable);

	Json Data;
	std::vector<Json> Datas;
	if (RegionNameResult.ResultIsNotNull())
	{
		Data["title"] = TCHAR_TO_UTF8("行业能耗占比-") + RegionNameResult.DataTable[0][0];
	}
	else
	{
		Data["title"] = TCHAR_TO_UTF8("行业能耗占比");

	}
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::EnergyConsumptionTrendsInThePast30Days(Json InData)
{
	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("近30天能耗趋势"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));
	TableTitle.push_back(TCHAR_TO_UTF8("电"));
	TableTitle.push_back(TCHAR_TO_UTF8("水"));
	TableTitle.push_back(TCHAR_TO_UTF8("气"));

	std::vector<std::string> Unit;
	Unit.push_back("(tce)");
	Unit.push_back(TCHAR_TO_UTF8("(kW·h)"));
	Unit.push_back(TCHAR_TO_UTF8("(m³)"));
	Unit.push_back(TCHAR_TO_UTF8("(m³)"));

	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(65,141,255,0)");

	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(65,141,255,1)");

	std::vector<std::string> HighColor;
	HighColor.push_back("#5ceaff");

	std::vector<std::string> GradientColor;
	GradientColor.push_back("#8FC31F");

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string RegionEnergyConsumptionCommand = "SELECT MONTH(CreateDate),DAY(CreateDate),SynEnergyConsum,ElecConsumSelf,WaterConsumSelf,GasConsumSelf FROM RegionEnergyEfficiencyDay WHERE RegionEnergyEfficiencyDay.RegionID='" + RegionID + "' AND RegionEnergyEfficiencyDay.CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' ORDER BY YEAR(CreateDate),MONTH(CreateDate),DAY(CreateDate);";

	FMysqlResult RegionEnergyConsumption = MaraidbConnector.Query(RegionEnergyConsumptionCommand);

	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{
		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			std::vector<Json>TableDatas;

			Json TableData;
			TableData["categoryName"] = TCHAR_TO_UTF8("供");
			TableData["start_color"] = StartColor[TableIndex % StartColor.size()];
			TableData["end_color"] = EndColor[TableIndex % EndColor.size()];
			TableData["high_color"] = HighColor[TableIndex % HighColor.size()];
			TableData["gradient_color"] = GradientColor[TableIndex % GradientColor.size()];

			std::vector<Json> CategoryDatas;
			for (auto RegionEnergyConsumptionRow : RegionEnergyConsumption.DataTable)
			{
				Json CategoryData;
				CategoryData["xLabel"] = RegionEnergyConsumptionRow[0] + "." + RegionEnergyConsumptionRow[1];
				CategoryData["yLabel"] = RegionEnergyConsumptionRow[(int)TableIndex + 2];
				CategoryDatas.push_back(CategoryData);
			}

			TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
			TableDatas.push_back(TableData);

			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[TableIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;
			Tables.push_back(Table);

		}
		Json Data;
		Data["title"] = DataTitle[DataIndex];
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;
}
Json OServerHelper::RegionalOverview(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string  RegionInfoCommand = "SELECT RegionName,Area FROM BaseRegionInfo WHERE BaseRegionInfo.RegionID='" + RegionID + "';";
	FMysqlResult RegionInfo = MaraidbConnector.Query(RegionInfoCommand);

	FMysqlResult EnteCount = MaraidbConnector.Query(str(boost::format("CALL GetRegionInfo_EnteNum('%1%')") % RegionID));
	FMysqlResult IndustryCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT IndustryID) FROM RegionIndustry WHERE FIND_IN_SET(RegionIndustry.RegionID,GetChildrenRegion('%1%'))") % RegionID));

	std::string RegionEnergyInfoCommand = "SELECT CarbonEmission,EnergyEcnoValueAdd,SynEnergyConsum FROM RegionEnergyEfficiencyMonth WHERE RegionID='" + RegionID + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
	FMysqlResult RegionEnergyInfo = MaraidbConnector.Query(RegionEnergyInfoCommand);

	Json Data;
	Data["value1"] = TCHAR_TO_UTF8("-- km²");
	Data["value2"] = TCHAR_TO_UTF8("-- T");
	Data["value3"] = TCHAR_TO_UTF8("-- 万元");
	Data["value6"] = TCHAR_TO_UTF8("-- tce");
	for (auto RegionEnergyInfoRow : RegionEnergyInfo.DataTable)
	{
		Data["value2"] = FillPlaceholderIfNull(ConvertToTenThousand(RegionEnergyInfoRow[0]), 2) + TCHAR_TO_UTF8(" 万t");
		Data["value3"] = FillPlaceholderIfNull(ConvertToTenThousand(RegionEnergyInfoRow[1], 2)) + TCHAR_TO_UTF8(" 万元");
		Data["value6"] = ConvertToTenThousand(RegionEnergyInfoRow[2]) + TCHAR_TO_UTF8(" 万tce");
	}
	if (RegionInfo.DataTable[0][1] != "")
	{
		Data["value1"] = SaveDecimalPlaces(RegionInfo.DataTable[0][1]) + TCHAR_TO_UTF8(" km²");
	}

	Data["value4"] = EnteCount.DataTable[0][0];
	Data["value5"] = IndustryCount.DataTable[0][0];


	Json ReturnData;
	ReturnData["title"] = RegionInfo.DataTable[0][0];
	ReturnData["data"] = Data;


	return ReturnData;
}
Json OServerHelper::KeyCompaniesMonitorEnergyConsumption(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("重点监控企业能耗"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));
	std::vector<std::string> Unit;
	Unit.push_back("(tce)");
	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, false, false, true,
		'-', EXLabelStepDateIterator::Day));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(65,140,255,1)");
	StartColor.push_back("rgba(209,246,255,1)");
	StartColor.push_back("rgba(0,215,229,0.5)");

	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(65,140,255,1)");
	EndColor.push_back("rgba(209,246,255,1)");
	EndColor.push_back("rgba(0,215,229,0.1)");

	std::vector<std::string> HighColor;
	HighColor.push_back("rgba(65,140,255,0.3)");
	HighColor.push_back("rgba(209,246,255,0.3)");
	HighColor.push_back("rgba(0,215,229,0.5)");


	std::vector<std::string> HighColor_1;
	HighColor_1.push_back("#5ceaff");

	std::vector<std::string> HighColor_2;
	HighColor_2.push_back("#5ceaff");

	std::vector<std::string> GradientColor;
	GradientColor.push_back("rgba(65,140,255,0)");
	GradientColor.push_back("rgba(209,246,255,0)");
	GradientColor.push_back("rgba(0,215,229,0.5)");


	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string TopCount = FIELDIFNULL(InData["TopCount"]);

	std::string EnteTopListCommand = "CALL GetEnteTopListByEnergyConsumption('" + RegionID + "','" + StartTime + "','" + EndTime + "','" + TopCount + "')";
	FMysqlResult EnteTopList = MaraidbConnector.Query(EnteTopListCommand);

	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{
		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{
			date Now = day_clock::local_day();

			std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			std::vector<Json>TableDatas;
			int CategoryIndex = 0;
			for (auto EnteTopListRow : EnteTopList.DataTable)
			{
				Json TableData;
				TableData["categoryName"] = EnteTopListRow[1];
				TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
				TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
				TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];
				TableData["gradient_color"] = GradientColor[CategoryIndex % GradientColor.size()];

				std::string EnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption_Day('" + EnteTopList.DataTable[CategoryIndex][0] + "','" + StartTime + "','" + EndTime + "')";
				FMysqlResult EnteEnergyConsumption = MaraidbConnector.Query(EnteEnergyConsumptionCommand);
				std::vector<Json> CategoryDatas;
				for (auto EnteEnergyConsumptionRow : EnteEnergyConsumption.DataTable)
				{
					boost::gregorian::date Date = boost::gregorian::from_string(EnteEnergyConsumptionRow[0]);
					Json CategoryData;
					CategoryData["xLabel"] = std::to_string(Date.day());
					CategoryData["yLabel"] = EnteEnergyConsumptionRow[(float)TableIndex + 1];
					CategoryDatas.push_back(CategoryData);
				}
				TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
				TableDatas.push_back(TableData);
				CategoryIndex++;
			}
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[TableIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
		}
		Json Data;
		Data["title"] = DataTitle[DataIndex];
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;
}

Json OServerHelper::RegionalAnomalyRanking(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> ExceptionEventCountCategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionExceptionEventCountCommand = "SELECT COUNT(DISTINCT ExceptionEventId) FROM AbnormalEnergyEvent,BaseEnteElecMeterInfo WHERE AbnormalEnergyEvent.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('" + ChildRegion[0] + "')) AND FilterDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
		FMysqlResult RegionExceptionEventCount = MaraidbConnector.Query(RegionExceptionEventCountCommand);

		Json ExceptionEventCountCategoryData;
		ExceptionEventCountCategoryData["xLabel"] = ChildRegion[1];
		ExceptionEventCountCategoryData["yLabel"] = RegionExceptionEventCount.DataTable[0][0];
		ExceptionEventCountCategoryDatas.push_back(ExceptionEventCountCategoryData);
	}

	Json ExceptionEventCountTableData;
	ExceptionEventCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	ExceptionEventCountTableData["start_color"] = "rgba(209,246,255,1)";
	ExceptionEventCountTableData["end_color"] = "rgba(209,246,255,1)";
	ExceptionEventCountTableData["high_color"] = "#5ceaff";
	std::sort(ExceptionEventCountCategoryDatas.begin(), ExceptionEventCountCategoryDatas.end(), SortCategoryDatas);

	ExceptionEventCountTableData["categoryData"] = ExceptionEventCountCategoryDatas;

	std::vector<Json>ExceptionEventCountTableDatas;
	ExceptionEventCountTableDatas.push_back(ExceptionEventCountTableData);

	Json ExceptionEventCountTable;
	ExceptionEventCountTable["title"] = TCHAR_TO_UTF8("总");
	ExceptionEventCountTable["unit"] = TCHAR_TO_UTF8("次");
	ExceptionEventCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(ExceptionEventCountCategoryDatas);
	ExceptionEventCountTable["data"] = ExceptionEventCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(ExceptionEventCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("区域异常排名");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::IndustryAnomalyRanking(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> ExceptionEventCountCategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		std::string IndustryExceptionEventCountCommand = "SELECT COUNT(DISTINCT ExceptionEventId) FROM AbnormalEnergyEvent,BaseEnteElecMeterInfo WHERE AbnormalEnergyEvent.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('" + RegionID + "')) AND BaseEnteElecMeterInfo.IndustryID= '" + ChildIndustry[0] + "' AND FilterDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
		FMysqlResult IndustryExceptionEventCount = MaraidbConnector.Query(IndustryExceptionEventCountCommand);

		Json ExceptionEventCountCategoryData;
		ExceptionEventCountCategoryData["xLabel"] = ChildIndustry[1];
		ExceptionEventCountCategoryData["yLabel"] = IndustryExceptionEventCount.DataTable[0][0];
		ExceptionEventCountCategoryDatas.push_back(ExceptionEventCountCategoryData);
	}

	Json ExceptionEventCountTableData;
	ExceptionEventCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	ExceptionEventCountTableData["start_color"] = "rgba(209,246,255,1)";
	ExceptionEventCountTableData["end_color"] = "rgba(209,246,255,1)";
	ExceptionEventCountTableData["high_color"] = "#5ceaff";
	std::sort(ExceptionEventCountCategoryDatas.begin(), ExceptionEventCountCategoryDatas.end(), SortCategoryDatas);

	ExceptionEventCountTableData["categoryData"] = ExceptionEventCountCategoryDatas;

	std::vector<Json>ExceptionEventCountTableDatas;
	ExceptionEventCountTableDatas.push_back(ExceptionEventCountTableData);

	Json ExceptionEventCountTable;
	ExceptionEventCountTable["title"] = TCHAR_TO_UTF8("总");
	ExceptionEventCountTable["unit"] = TCHAR_TO_UTF8("次");
	ExceptionEventCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(ExceptionEventCountCategoryDatas);
	ExceptionEventCountTable["data"] = ExceptionEventCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(ExceptionEventCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("行业异常排名");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ListOfAbnormalEvents(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string AbnormalEventsListCommand = "CALL GetExceptionEventList('" + RegionID + "', '" + StartTime + "', '" + EndTime + "')";
	FMysqlResult AbnormalEventsList = MaraidbConnector.Query(AbnormalEventsListCommand);

	std::vector<Json> RowHeader;
	RowHeader.push_back("{\"headerName\":\"异常编码\",\"width\":\"15\"}"_json);
	RowHeader.push_back("{\"headerName\":\"企业简称\",\"width\":\"15\"}"_json);
	RowHeader.push_back("{\"headerName\":\"用户电号\",\"width\":\"10\"}"_json);
	RowHeader.push_back("{\"headerName\":\"行业分类\",\"width\":\"10\"}"_json);
	RowHeader.push_back("{\"headerName\":\"预判原因\",\"width\":\"5\"}"_json);
	RowHeader.push_back("{\"headerName\":\"确认原因\",\"width\":\"5\"}"_json);
	RowHeader.push_back("{\"headerName\":\"识别日期\",\"width\":\"8\"}"_json);

	std::vector<Json> TableDatas;
	for (auto AbnormalEventsListRow : AbnormalEventsList.DataTable)
	{
		std::vector<Json> Datas;
		for (auto Value : AbnormalEventsListRow)
		{
			Json Data;
			Data["value"] = Value;
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("异常用能事件列表");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;

}
Json OServerHelper::AIJudgmentOfAbnormalCause(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ExceptionEventId = InData["ExceptionEventId"];

	std::string AIResultCommand = "SELECT AbnormalEnergyEvent.ExceptionResult FROM AbnormalEnergyEvent WHERE AbnormalEnergyEvent.ExceptionEventId='" + ExceptionEventId + "';";
	FMysqlResult Result = MaraidbConnector.Query(AIResultCommand);
	if (Result.DataTable.size() <= 0)
	{
		return {};
	}
	Json AIResult = Json::parse(Result.DataTable[0][0]);
	std::vector<Json> CategoryDatas;
	for (auto ResultRow : AIResult)
	{
		Json CategoryData;
		CategoryData["xLabel"] = ResultRow["reasonName"];
		CategoryData["yLabel"] = ResultRow["modelResult"];
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("异常原因");
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = "";
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("异常原因AI判断");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::AnnualAbnormalCauseChangeAnalysis(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::vector<std::string> StartColor;
	StartColor.push_back("rgb(209,246,255)");
	StartColor.push_back("rgb(145,255,85)");
	StartColor.push_back("rgb(255,213,176)");
	StartColor.push_back("rgb(97,173,255)");
	StartColor.push_back("rgb(255,255,255)");

	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(65,141,255,0.6)");
	EndColor.push_back("rgba(255,132,0,0.6)");
	EndColor.push_back("rgba(145,255,85,0.6)");

	std::vector<std::string> HighColor;
	HighColor.push_back("rgba(209,246,255,0.3)");
	HighColor.push_back("rgba(145,255,85,0.3)");
	HighColor.push_back("rgba(255,213,176,0.3)");

	std::vector<std::string> GradientColor;
	GradientColor.push_back("rgba(209,246,255,0.05)");
	GradientColor.push_back("rgba(145,255,85,0.05)");
	GradientColor.push_back("rgba(97,173,255,0.05)");
	GradientColor.push_back("rgba(255,213,176,0.05)");

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string AiResultCommand = "SELECT AiResult FROM AbnormalEnergyEvent GROUP BY AiResult;";
	FMysqlResult AiResultList = MaraidbConnector.Query(AiResultCommand);


	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	std::vector<Json> TableDatas;
	int CategoryIndex = 0;
	for (auto AiResultListRow : AiResultList.DataTable)
	{
		std::string EventDistributionCommand = "SELECT YEAR(FilterDate),MONTH(FilterDate),COUNT(DISTINCT ExceptionEventId) FROM AbnormalEnergyEvent,BaseEnteElecMeterInfo WHERE AiResult='" + AiResultListRow[0] + "' AND AbnormalEnergyEvent.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('" + RegionID + "')) AND FilterDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY YEAR(FilterDate), MONTH(FilterDate) ORDER BY YEAR(FilterDate),MONTH(FilterDate)";
		FMysqlResult EventDistribution = MaraidbConnector.Query(EventDistributionCommand);

		std::vector<Json> CategoryDatas;
		for (auto EventDistributionRow : EventDistribution.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = EventDistributionRow[0] + "." + EventDistributionRow[1];
			CategoryData["yLabel"] = EventDistributionRow[2];
			CategoryDatas.push_back(CategoryData);
		}
		Json TableData;
		TableData["categoryName"] = AiResultListRow[0];
		TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
		TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
		TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];
		TableData["gradient_color"] = GradientColor[CategoryIndex % GradientColor.size()];
		TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
		TableDatas.push_back(TableData);
		CategoryIndex++;
	}
	std::vector<Json> Tables;
	Json Table;

	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = TCHAR_TO_UTF8("(次)");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("年度异常原因变化分析");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::EnterpriseAnomalies(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ExceptionEventId = FIELDIFNULL(InData["ExceptionEventId"]);

	FMysqlResult ElecMetID = MaraidbConnector.Query(str(boost::format("SELECT ElecMetID FROM AbnormalEnergyEvent WHERE ExceptionEventId='%1%';") % ExceptionEventId));
	if (ElecMetID.DataTable.size() <= 0)
	{
		return {};
	}
	FMysqlResult AbnormalEventsList = MaraidbConnector.Query(str(boost::format("SELECT ExceptionEventId,AiResult,FilterDate,ReviewDate,ConfirmReasonName FROM AbnormalEnergyEvent WHERE ElecMetID='%1%';") % ElecMetID.DataTable[0][0]));

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"异常编号\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"预判原因\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"识别日期\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"复核日期\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"复查结果\"}")));

	std::vector<Json> TableDatas;

	for (auto AbnormalEventsListRow : AbnormalEventsList.DataTable)
	{
		std::vector<Json> TableData;
		for (auto AbnormalEventsListRowValue : AbnormalEventsListRow)
		{
			TableData.push_back({ {"value",AbnormalEventsListRowValue} });
		}
		TableDatas.push_back(TableData);
	}
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	std::vector<Json> Datas;
	Data["title"] = TCHAR_TO_UTF8("企业异常事件");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ManualReviewResults(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ExceptionEventId = FIELDIFNULL(InData["ExceptionEventId"]);
	std::string Command = "SELECT ContactPerson,ContactPhone,ConfirmReasonName,ConfirmUser,ReviewDate,ReviewEnteName,ConfirmRemark FROM AbnormalEnergyEvent LEFT JOIN BaseEnteElecMeterInfo ON BaseEnteElecMeterInfo.ElecMetID=AbnormalEnergyEvent.ElecMetID LEFT JOIN BaseEnteInfo ON BaseEnteInfo.ElecMetID=BaseEnteElecMeterInfo.ElecMetID WHERE AbnormalEnergyEvent.ExceptionEventId='" + ExceptionEventId + "';";
	FMysqlResult Result = MaraidbConnector.Query(Command);
	Json Data;
	Data[TCHAR_TO_UTF8("联系人")] = "";
	Data[TCHAR_TO_UTF8("联系方式")] = "";
	Data[TCHAR_TO_UTF8("确认原因")] = "";
	Data[TCHAR_TO_UTF8("复查人")] = "";
	Data[TCHAR_TO_UTF8("复查日期")] = "";
	Data[TCHAR_TO_UTF8("核查单位")] = "";
	Data[TCHAR_TO_UTF8("备注说明")] = "";
	for (auto ResultRow : Result.DataTable)
	{
		Data[TCHAR_TO_UTF8("联系人")] = ResultRow[0];
		Data[TCHAR_TO_UTF8("联系方式")] = ResultRow[1];
		Data[TCHAR_TO_UTF8("确认原因")] = ResultRow[2];
		Data[TCHAR_TO_UTF8("复查人")] = ResultRow[3];
		Data[TCHAR_TO_UTF8("复查日期")] = ResultRow[4];
		Data[TCHAR_TO_UTF8("核查单位")] = ResultRow[5];
		Data[TCHAR_TO_UTF8("备注说明")] = ResultRow[6];
	}
	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("人工复检结果");
	ReturnData["data"] = Data;

	return ReturnData;
}

Json OServerHelper::ReportOrderService(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult OrderCountAndMoney = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT OrderCode),SUM(OrderMoney) FROM ServerApply WHERE OrderStatus='已上传成果' AND PayDate BETWEEN '%1%' AND '%2%';") % StartTime % EndTime)));
	FMysqlResult PendingToAduitOrderCount = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT OrderCode) FROM ServerApply WHERE OrderStatus='待付款';"))));

	std::vector<Json> RowHeaders;

	if (OrderCountAndMoney.ResultIsNotNull())
	{
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("本月订单")},{"value",OrderCountAndMoney.DataTable[0][0]} });
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("本月金额")},{"value",ConvertToTenThousand(OrderCountAndMoney.DataTable[0][1])} });
	}
	if (PendingToAduitOrderCount.ResultIsNotNull())
	{
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("待付款订单")},{"value",PendingToAduitOrderCount.DataTable[0][0]} });
	}
	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeaders;

	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务概况");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}
Json OServerHelper::ReportOrderServiceResult(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);

	std::string ReportOrderServiceListCommand = "SELECT ServerApplyResult.ReportCode,ServerApplyResult.ReportName,ServerApplyResult.ReportType,ServerApplyResult.ArchDate FROM ServerApplyResult,ServerApply\
												WHERE ServerApplyResult.OrderCode = ServerApply.OrderCode\
												AND ServerApply.EnteID = (SELECT ServerApply.EnteID FROM ServerApply WHERE ServerApply.OrderCode = '" + ApplyCode + "')\
												GROUP BY ServerApplyResult.ArchDate";


	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;


	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"报告编号\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"报告名称\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"报告类型\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"接收日期\",\"width\":\"25\"}")));


	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 3)
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);

	}

	while (TableDatas.size() < 9)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = TCHAR_TO_UTF8("-----");
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单成果");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::ReportOrderService_OrderStatus(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string Status[7] = { TCHAR_TO_UTF8("待付款"),TCHAR_TO_UTF8("已付款，编制中"),TCHAR_TO_UTF8("已上传成果"),TCHAR_TO_UTF8("已到期"),TCHAR_TO_UTF8("已关闭"),TCHAR_TO_UTF8("退款申请审核中"),TCHAR_TO_UTF8("退款成功,已关闭")};

	std::vector<Json> TableDatas;

	for (int i = 0; i < 7; i++)
	{
		FMysqlResult Count = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT OrderCode) FROM ServerApply WHERE OrderStatus='%1%';") % Status[i]));
		Json TableData;
		TableData["index"] = i;
		TableData["statusName"] = Status[i];
		TableData["value"] = boost::lexical_cast<int>(Count.DataTable[0][0]);
		TableDatas.push_back(TableData);
	}
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("订单状态");
	Data["data"] = TableDatas;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ReportOrderService_IndustryComparison(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult ChildIndustryList = MaraidbConnector.Query("SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE BaseIndustryInfo.ParentID='' OR ISNULL(BaseIndustryInfo.ParentID);");

	std::vector<Json> CategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		FMysqlResult Count = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT OrderCode) FROM ServerApply,BaseEnteElecMeterInfo WHERE OrderStatus='已上传成果' AND PayDate BETWEEN '%1%' AND '%2%' AND ServerApply.EnteID=BaseEnteElecMeterInfo.EnteID AND IndustryID ='%3%'") % StartTime % EndTime % ChildIndustry[0]));

		Json CategoryData;
		CategoryData["xLabel"] = ChildIndustry[1];
		CategoryData["yLabel"] = Count.DataTable[0][0];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.25)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务-行业对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::ReportOrderService_TypeDistribution(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult Result = MaraidbConnector.Query(str(boost::format("SELECT OrderStatus,COUNT(DISTINCT OrderCode) FROM ServerApply WHERE OrderDate BETWEEN '%1%' AND '%2%' GROUP BY OrderStatus;") % StartTime % EndTime));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		CategoryDatas.push_back({ {"xLabel",ResultRow[0]},{"yLabel",ResultRow[1]} });
	}
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json> TablesDatas;
	TablesDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["data"] = TablesDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务状态分布");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	;
	return ReturnData;
}
Json OServerHelper::ReportOrderService_RegionalComparison(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult ChildRegionList = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='%1%';") % RegionID)));

	std::vector<Json> CategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT OrderCode) FROM ServerApply,BaseEnteInfo WHERE OrderStatus='已上传成果' AND PayDate BETWEEN '%1%' AND '%2%' AND ServerApply.EnteID=BaseEnteInfo.EnteID AND FIND_IN_SET(RegionID,GetChildrenRegion('%3%'));") % StartTime % EndTime % ChildRegion[0])));

		Json CategoryData;
		CategoryData["xLabel"] = ChildRegion[1];
		CategoryData["yLabel"] = RegionEnteCount.DataTable[0][0];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatasReverse);

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.25)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务-区域对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::ReportOrderServiceTrend(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT MONTH(PayDate),DAY(PayDate),COUNT(DISTINCT PayDate) FROM ServerApply WHERE OrderStatus='已上传成果' AND PayDate BETWEEN  '%1%' AND '%2%' GROUP BY YEAR(PayDate),MONTH(PayDate),DAY(PayDate) ORDER BY PayDate") % StartTime % EndTime)));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = ResultRow[0] + "." + ResultRow[1];
		CategoryData["yLabel"] = ResultRow[2];
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.3)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "rgba(97,174,255,1)";
	TableData["gradient_color"] = "rgba(97,174,255,1)";
	TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务趋势");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::ReportOrderServiceDetail(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);
	std::string ReportOrderServiceListCommand = "SELECT ServerApply.OrderName, BaseEnteInfo.EnteName, BaseEnteInfo.Address, ServerApply.OrderMoney, ServerApply.OrderDate, ServerApply.OrderDetail, ServerApply.OrderCode, BaseEnteInfo.ContactPerson, BaseEnteInfo.ContactPhone, ServerApply.PayDate, ServerApply.LimitDate\
												FROM ServerApply, BaseEnteInfo\
												WHERE ServerApply.EnteID = BaseEnteInfo.EnteID\
												AND ServerApply.OrderCode ='" + ApplyCode + "';";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;
	std::string FirstHalfTitle;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		for (auto Value : ReportOrderServiceListRow)
		{
			Json CurValue;
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
		}
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = "";
	return TableData;
}
Json OServerHelper::ReportOrderService_MonthlyConsumption(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT (@i:=@i+1)i,EnteName,Money FROM (select @i:=0) AS A,(SELECT EnteName,SUM(OrderMoney) AS Money FROM ServerApply,BaseEnteInfo  WHERE OrderStatus='已上传成果' AND PayDate BETWEEN '%1%' AND '%2%' AND BaseEnteInfo.EnteID=ServerApply.EnteID GROUP BY EnteName ORDER BY SUM(OrderMoney)DESC) AS T;") % StartTime % EndTime)));

	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("排名"),TCHAR_TO_UTF8("企业名称"),TCHAR_TO_UTF8("月消费") };
	std::vector<std::string > RowWidth = { "10" ,"10" ,"10" };

	Json Table = FillTableJson(RowHeader, RowWidth, Result.DataTable,8);
	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务消费月排行");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ReportOrderService_UserHistory(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);

	std::string ReportOrderServiceListCommand = "SELECT ServerApply.OrderCode,ServerApply.OrderName,ServerApply.OrderDate FROM ServerApply\
												WHERE ServerApply.EnteID = (SELECT ServerApply.EnteID FROM ServerApply WHERE ServerApply.OrderCode = '" + ApplyCode + "')\
												GROUP BY ServerApply.OrderDate; ";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;

	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"报告编码\",\"width\":\"33\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"报告名称\",\"width\":\"33\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"接收日期\",\"width\":\"33\"}")));


	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 2 && Data["value"] != "")
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);

	}

	while (TableDatas.size() < 9)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = TCHAR_TO_UTF8("-----");
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("用户历史订单");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}

Json OServerHelper::CustomizedService(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult OrderCountAndMoney = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT ApplyID),SUM(OfferPrice) FROM CustApply WHERE ApplyStatus='编制完成'AND PayTime BETWEEN '%1%' AND '%2%';") % StartTime % EndTime)));
	FMysqlResult PendingToAduitOrderCount = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT ApplyID) FROM CustApply WHERE ApplyStatus='受理中';"))));

	std::vector<Json> RowHeaders;

	if (OrderCountAndMoney.ResultIsNotNull())
	{
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("本月订单")},{"value",OrderCountAndMoney.DataTable[0][0]} });
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("本月金额")},{"value",ConvertToTenThousand(OrderCountAndMoney.DataTable[0][1])} });
	}
	if (PendingToAduitOrderCount.ResultIsNotNull())
	{
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("待受理订单")},{"value",PendingToAduitOrderCount.DataTable[0][0]} });
	}
	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeaders;

	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("定制服务概况");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::CustomizedService_OrderResults(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);

	std::string ReportOrderServiceListCommand = "SELECT CustApplyResult.OrderCode,CustApplyResult.PlanName,CustApplyResult.PlanType,CustApplyResult.ReceiveDate FROM CustApplyResult,BaseEnteElecMeterInfo,CustApply\
												WHERE CustApplyResult.OrderCode = CustApply.OrderCode\
												AND CustApply.ApplyEnteID = (SELECT CustApply.ApplyEnteID FROM CustApply WHERE CustApply.ApplyCode = '" + ApplyCode + "')\
												GROUP BY CustApplyResult.ReceiveDate";


	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;



	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"成果编码\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"成果名称\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"成果类型\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"接收日期\",\"width\":\"25\"}")));


	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 3)
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);

	}

	while (TableDatas.size() < 4)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = TCHAR_TO_UTF8("-----");
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("订单成果");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::CustomizedService_OrderStatus(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string Status[7] = { TCHAR_TO_UTF8("受理中"),TCHAR_TO_UTF8("待付款"),TCHAR_TO_UTF8("已付款，编制中"),TCHAR_TO_UTF8("编制完成"),TCHAR_TO_UTF8("已关闭"),TCHAR_TO_UTF8("退款申请审核中"),TCHAR_TO_UTF8("退款成功，已关闭") };

	std::vector<Json> TableDatas;

	for (int i = 0; i < 7; i++)
	{
		FMysqlResult Count = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ApplyID) FROM CustApply WHERE ApplyStatus='%1%';") % Status[i]));
		Json TableData;
		TableData["index"] = i;
		TableData["statusName"] = Status[i];
		TableData["value"] = boost::lexical_cast<int>(Count.DataTable[0][0]);
		TableDatas.push_back(TableData);
	}
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("订单状态");
	Data["data"] = TableDatas;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::CustomizedService_TypeDistribution(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT ServiceType,COUNT(DISTINCT ApplyID) FROM CustApply WHERE ApplyStatus='编制完成' AND PayTime BETWEEN '%1%' AND '%2%' GROUP BY ServiceType;") % StartTime % EndTime)));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		CategoryDatas.push_back({ {"xLabel",ResultRow[0]},{"yLabel",ResultRow[1]} });
	}
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json> TablesDatas;
	TablesDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["data"] = TablesDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("个性化定制服务状态分布");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	;
	return ReturnData;
}
Json OServerHelper::CustomizedServiceTrend(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT MONTH(PayTime),DAY(PayTime),COUNT(DISTINCT ApplyID) FROM CustApply WHERE ApplyStatus='编制完成' AND PayTime BETWEEN  '%1%' AND '%2%' GROUP BY YEAR(PayTime),MONTH(PayTime),DAY(PayTime) ORDER BY PayTime") % StartTime % EndTime)));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = ResultRow[0] + "." + ResultRow[1];
		CategoryData["yLabel"] = ResultRow[2];
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.3)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "rgba(97,174,255,1)";
	TableData["gradient_color"] = "rgba(97,174,255,1)";
	TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("个性化定制服务趋势");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::CustomizedService_UserHistory(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);

	std::string ReportOrderServiceListCommand = "SELECT CustApply.ApplyCode,CustApply.ApplyServiceName,CustApply.CreateDate FROM CustApply\
												WHERE CustApply.ApplyEnteID = (SELECT CustApply.ApplyEnteID FROM CustApply WHERE CustApply.ApplyCode = '" + ApplyCode + "')\
												GROUP BY CustApply.CreateDate; ";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;



	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"订单编码\",\"width\":\"33\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"服务名称\",\"width\":\"33\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"接收日期\",\"width\":\"33\"}")));


	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 2 && Data["value"] != "")
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);

	}


	while (TableDatas.size() < 9)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("用户历史订单");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::AnnualComparisonOfServiceAmount(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	FMysqlResult CustServiceNum = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT YEAR(PayTime),MONTH(PayTime),SUM(OfferPrice) FROM CustApply WHERE ApplyStatus='编制完成' AND PayTime BETWEEN '%1%' AND '%2%' GROUP BY YEAR(PayTime),MONTH(PayTime);") % StartTime % EndTime)));
	FMysqlResult OrderServiceNum = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT YEAR(PayDate),MONTH(PayDate),SUM(OrderMoney) FROM ServerApply WHERE OrderStatus='已上传成果' AND PayDate BETWEEN '%1%' AND '%2%' GROUP BY YEAR(PayDate),MONTH(PayDate);") % StartTime % EndTime)));

	FMysqlResult Result[3] = { CustServiceNum,OrderServiceNum };

	std::string CategoryNames[2] = { TCHAR_TO_UTF8("个性化定制服务"),TCHAR_TO_UTF8("报告订单服务") };
	std::string StartColor[2] = { "rgba(255,255,255,0)","rgba(65,141,255,0)" };
	std::string EndColor[2] = { "rgba(255,255,255,1)","rgba(65,141,255,1)" };

	std::vector<Json> TableDatas;
	for (int i = 0; i < 2; i++)
	{
		double Totle = 0;
		std::vector<Json> CategoryDatas;
		for (auto ResultRow : Result[i].DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = ResultRow[0] + "." + ResultRow[1];
			CategoryData["yLabel"] = ConvertToTenThousand(ResultRow[2]);
			CategoryDatas.push_back(CategoryData);
			try
			{
				Totle += boost::lexical_cast<double>(ResultRow[2]);
			}
			catch (const std::exception &)
			{
				std::cout << "AnnualComparisonOfServiceAmount: ResultRow is" << ResultRow[2] << std::endl;
			}
		}
		Json TableData;
		TableData["categoryName"] = CategoryNames[i];
		TableData["start_color"] = StartColor[i];
		TableData["end_color"] = EndColor[i];
		TableData["high_color1"] = "#F2F2F2";
		TableData["high_color2"] = "#fce700";
		TableData["totals"] = ConvertToTenThousand(std::to_string(Totle));
		TableData["unit"] = TCHAR_TO_UTF8("万元");
		TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

		TableDatas.push_back(TableData);
	}

	Json Table;
	Table["title"] = "";
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] =TCHAR_TO_UTF8("服务金额年度对比");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AnnualComparisonOfNumberOfServices(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	FMysqlResult AccServiceNum = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT YEAR(PayTime),MONTH(PayTime),COUNT(DISTINCT ApplyID) FROM AccApply WHERE ApplyStatus='已开通' AND PayTime BETWEEN '%1%' AND '%2%' GROUP BY YEAR(PayTime),MONTH(PayTime);") % StartTime % EndTime)));
	FMysqlResult CustServiceNum = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT YEAR(PayTime),MONTH(PayTime),COUNT(DISTINCT ApplyID) FROM CustApply WHERE ApplyStatus='编制完成' AND PayTime BETWEEN '%1%' AND '%2%' GROUP BY YEAR(PayTime),MONTH(PayTime);") % StartTime % EndTime)));
	FMysqlResult OrderServiceNum = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT YEAR(PayDate),MONTH(PayDate),COUNT(DISTINCT OrderCode) FROM ServerApply WHERE OrderStatus='已上传成果' AND PayDate BETWEEN '%1%' AND '%2%' GROUP BY YEAR(PayDate),MONTH(PayDate);") % StartTime % EndTime)));

	FMysqlResult Result[3] = { AccServiceNum,CustServiceNum,OrderServiceNum };

	std::string CategoryNames[3] = { TCHAR_TO_UTF8("数据接入服务"),TCHAR_TO_UTF8("个性化定制服务"),TCHAR_TO_UTF8("报告订单服务") };
	std::string StartColor[3] = { "rgba(1,254,234,0)","rgba(255,255,255,0)","rgba(65,141,255,0)" };
	std::string EndColor[3] = { "rgba(1,254,234,1)","rgba(255,255,255,1)","rgba(65,141,255,1)" };

	std::vector<Json> TableDatas;
	for (int i = 0; i < 3; i++)
	{
		int Totle = 0;
		std::vector<Json> CategoryDatas;
		for (auto ResultRow : Result[i].DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = ResultRow[0] + "." + ResultRow[1];
			CategoryData["yLabel"] = ResultRow[2];
			CategoryDatas.push_back(CategoryData);
			try
			{
				Totle += boost::lexical_cast<int>(ResultRow[2]);
			}
			catch (const std::exception &)
			{
				std::cout << "AnnualComparisonOfNumberOfServices: ResultRow is" << ResultRow[2] << std::endl;
			}
		}
		Json TableData;
		TableData["categoryName"] = CategoryNames[i];
		TableData["start_color"] = StartColor[i];
		TableData["end_color"] = EndColor[i];
		TableData["high_color1"] = "#F2F2F2";
		TableData["high_color2"] = "#fce700";
		TableData["totals"] = std::to_string(Totle);
		TableData["unit"] = TCHAR_TO_UTF8("次");
		TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

		TableDatas.push_back(TableData);
	}

	Json Table;
	Table["title"] = "";
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("服务数量年度对比");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::AccessService_OrderStatus(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string Status[8] = { TCHAR_TO_UTF8("受理中"),TCHAR_TO_UTF8("待付款"),TCHAR_TO_UTF8("已付款，实施中"),TCHAR_TO_UTF8("已开通"),TCHAR_TO_UTF8("已拒绝"),TCHAR_TO_UTF8("已关闭"),TCHAR_TO_UTF8("退款申请审核中"),TCHAR_TO_UTF8("退款成功，已关闭") };

	std::vector<Json> TableDatas;

	for (int i = 0; i < 8; i++)
	{

		FMysqlResult Count = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ApplyID) FROM AccApply WHERE ApplyStatus='%1%';") % Status[i]));
		Json TableData;
		TableData["index"] = i;
		TableData["statusName"] = Status[i];
		TableData["value"] = boost::lexical_cast<int>(Count.DataTable[0][0]);
		TableDatas.push_back(TableData);
	}
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("订单状态");
	Data["data"] = TableDatas;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AccessService_IndustryComparison(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime =FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult ChildIndustryList = MaraidbConnector.Query("SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE BaseIndustryInfo.ParentID='' OR ISNULL(BaseIndustryInfo.ParentID);");

	std::vector<Json> CategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		FMysqlResult Count = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT ApplyCode) FROM AccApply,BaseEnteElecMeterInfo WHERE ApplyStatus='已开通' AND AccApply.PayTime BETWEEN '%1%' AND '%2%' AND ApplyEnteID=EnteID AND IndustryID ='%3%'") % StartTime % EndTime % ChildIndustry[0])));

		Json CategoryData;
		CategoryData["xLabel"] = ChildIndustry[1];
		CategoryData["yLabel"] = Count.DataTable[0][0];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.25)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("数据接入服务-行业对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::AccessService_TypeDistribution(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult Result = MaraidbConnector.Query(str(boost::format("SELECT ApplyStatus,COUNT(DISTINCT ApplyID) FROM AccApply WHERE CreateDate BETWEEN '%1%' AND '%2%' OR PayTime BETWEEN '%1%' AND '%2%' GROUP BY ApplyStatus;") % StartTime % EndTime));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		CategoryDatas.push_back({ {"xLabel",ResultRow[0]},{"yLabel",ResultRow[1]} });
	}
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json> TablesDatas;
	TablesDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["data"] = TablesDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("本月数据接入服务状态分布");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	;
	return ReturnData;
}
Json OServerHelper::AccessService_RegionalComparison(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> CategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionEnteCountCommand = TCHAR_TO_UTF8("SELECT COUNT(DISTINCT ApplyCode) FROM AccApply,BaseEnteInfo WHERE ApplyStatus='已开通' AND AccApply.PayTime BETWEEN '" + StartTime + "' AND '" + EndTime + "' AND ApplyEnteID=EnteID AND FIND_IN_SET(RegionID,GetChildrenRegion('" + ChildRegion[0] + "'));");
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(RegionEnteCountCommand);

		Json CategoryData;
		CategoryData["xLabel"] = ChildRegion[1];
		CategoryData["yLabel"] = RegionEnteCount.DataTable[0][0];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatasReverse);

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.25)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("数据接入服务-区域对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::AccessService(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EnteCount = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT ApplyEnteID) FROM AccApply WHERE ApplyStatus='已开通'AND PayTime BETWEEN '%1%' AND '%2%';") % StartTime % EndTime)));
	FMysqlResult ElecMetIDList = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT ElecMetID FROM AccApply WHERE ApplyStatus='已开通'AND PayTime BETWEEN '%1%' AND '%2%';") % StartTime % EndTime)));
	std::vector<Json> RowHeaders;

	if (EnteCount.ResultIsNotNull())
	{
		RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("本月接入企业")},{"value",EnteCount.DataTable[0][0]} });
	}
	int MetIDCount = 0;
	std::vector<std::string> ElecMetIDs;
	for (auto ElecMetIDRow: ElecMetIDList.DataTable)
	{
		std::vector<std::string> ElecMetIDVector;
		boost::split(ElecMetIDVector, ElecMetIDRow[0], boost::is_any_of(","), boost::token_compress_on);
		ElecMetIDs.insert(ElecMetIDs.end(), ElecMetIDVector.begin(), ElecMetIDVector.end());
	}
	std::set<std::string>ElecMetIDSet(ElecMetIDs.begin(), ElecMetIDs.end());
	ElecMetIDs.assign(ElecMetIDSet.begin(), ElecMetIDSet.end());

	RowHeaders.push_back({ {"headerName",TCHAR_TO_UTF8("本月接入户号")},{"value",ElecMetIDs.size()} });

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeaders;

	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("数据接入服务概况");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AccessServiceTrend(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT MONTH(PayTime),DAY(PayTime),COUNT(DISTINCT ApplyID) FROM AccApply WHERE ApplyStatus='已开通' AND AccApply.PayTime BETWEEN  '%1%' AND '%2%' GROUP BY YEAR(PayTime),MONTH(PayTime),DAY(PayTime) ORDER BY PayTime") % StartTime % EndTime)));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = ResultRow[0] + "." + ResultRow[1];
		CategoryData["yLabel"] = ResultRow[2];
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.3)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "rgba(97,174,255,1)";
	TableData["gradient_color"] = "rgba(97,174,255,1)";
	TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("数据接入服务趋势");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::AccessServiceLatest(Json InData) {
	using namespace std;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT DISTINCT ApplyEnteName,ApplyAccName FROM AccApply WHERE ApplyStatus='已开通' Order By AccApply.PayTime DESC Limit 50 ;"))));

	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("企业名称") ,TCHAR_TO_UTF8("服务名称") };
	std::vector<std::string > RowWidth = { "10" ,"10" };

	Json Table = FillTableJson(RowHeader, RowWidth, Result.DataTable,8);
	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("最新接入企业");
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::RespondingEnterprise(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	std::string sql = "SELECT EnteID FROM VppEventEnte WHERE FeedbackStatus='"+TCHAR_TO_UTF8('等待回复')+"'";
	auto res1 = MaraidbConnector.Query(sql);
	for (std::vector<std::string> r1 : res1.DataTable) {
		Json list;
		auto res2 = MaraidbConnector.Query("SELECT EnteName, ContactPerson, ContactPhone FROM BaseEnteInfo WHERE EnteID=" + r1[0]);
		for (std::vector<std::string> r2 : res2.DataTable) {
			list["wsgvse"] = r2[0];//企业名称
			list["qafves"] = r2[1];//联系人
			list["nete"] = r2[2];//联系电话
			data.push_back(list);
		}
	}
	tableData["title"] = "rto";//待应答企业
	tableData["data"] = data;
	return tableData;
}
Json OServerHelper::RealTimeFullCaliberLoad(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string RegionID = boost::lexical_cast<std::string>(FIELDIFNULL(InData["RegionID"]));
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> yLabelRang;
	for (int i = 0; i <= 24; i++) {
		Json list1;
		Json list2;
		list1["label"] = boost::lexical_cast<std::string>(i) + ":00";
		list2["label"] = i * 5;
		xLabelRang.push_back(list1);
		yLabelRang.push_back(list2);
	}
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryDatas;
	if (RegionID != "") {
		auto nameResult = MaraidbConnector.Query("SELECT CreateDate, SupportLoad FROM RegionElecInfo WHERE RegionID=" + RegionID + " ORDER BY CreateDate ASC");
		for (std::vector<std::string> nameResultData : nameResult.DataTable) {
			Json list;
			list["xLabel"] = nameResultData[0];
			list["yLabel"] = nameResultData[1];
			categoryDatas.push_back(list);
		}
	}
	data1Json["categoryName"] = "gwevs";//预计负载
	data1Json["lineStyle"] = "solid";
	data1Json["start_color"] = "rgba(208,245,253,1)";
	data1Json["end_color"] = "rgba(208,245,253,1)";
	data1Json["high_color"] = "rgba(208,245,253,1)";
	data1Json["gradient_color"] = "rgba(208,245,253,1)";
	data1Json["categoryData"] = categoryDatas;
	data1.push_back(data1Json);
	tableJson["title"] = "vnso";//总
	tableJson["unit"] = "(kw)";
	tableJson["xLabelRang"] = xLabelRang;
	tableJson["yLabelRang"] = yLabelRang;
	tableJson["data"] = data1;
	table.push_back(tableJson);
	dataJson["title"] = "weoifjo";//实时全口径负荷
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}
Json OServerHelper::EventCompletion(Json InData) {
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	FMysqlResult EventOverview = MaraidbConnector.Query(str(boost::format("SELECT TotolPlan,TotalComplete FROM VppEvent WHERE EventCode='%1%';") % EventCode));

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	int EnteCount = 0;
	std::string TotlePlan = "--";
	std::string TotalComplete = "--";

	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
			EnteCount++;
	}
	if (EventOverview.ResultIsNotNull())
	{
		TotlePlan = FillPlaceholderIfNull(EventOverview.DataTable[0][0], 2);
		TotalComplete = FillPlaceholderIfNull(EventOverview.DataTable[0][1], 2);
	}
	std::vector<Json> Tables;
	Tables.push_back({ {"title",TCHAR_TO_UTF8("计划总量")},{"xLabel","kW"},{"yLabel",TotlePlan} });
	Tables.push_back({ {"title",TCHAR_TO_UTF8("参与企业")},{"xLabel",TCHAR_TO_UTF8("家")},{"yLabel",std::to_string(EnteCount)} });
	Tables.push_back({ {"title",TCHAR_TO_UTF8("完成总量")},{"xLabel","kW"},{"yLabel",TotalComplete} });

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("事件完成情况");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::PeakCutandValleyFillingDetails(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string EventDetails = "Select EventCode,EventStatus,EventType,PrepareTime,PriceTactic,TotolMoney,TotolPlan,TotalComplete,StartDate,EndDate From VppEvent Where EventCode = '" + EventCode + "';";
	FMysqlResult EventDetailsResult = MaraidbConnector.Query(EventDetails);
	std::vector<Json> Datas;

	std::string Keys[10] = { TCHAR_TO_UTF8("事件编号"),TCHAR_TO_UTF8("事件状态"),TCHAR_TO_UTF8("事件类型"),TCHAR_TO_UTF8("准备时长"),TCHAR_TO_UTF8("价格策略"),TCHAR_TO_UTF8("总激励金"),TCHAR_TO_UTF8("计划总量"),TCHAR_TO_UTF8("完成总量"),TCHAR_TO_UTF8("开始时间"),TCHAR_TO_UTF8("结束时间") };
	std::string Units[10] = { "","","","h","",TCHAR_TO_UTF8("元"),"kW","kW","","" };


	for (int Index=0;Index<10;Index++)
	{
		Json Data;
		Data["key"] = Keys[Index];
		Data["value"] = EventDetailsResult.ResultIsNotNull()? (EventDetailsResult.DataTable[0][Index]+ Units[Index]) : TCHAR_TO_UTF8("------");
		Datas.push_back(Data);
	}

	Json ReturnJson;
	ReturnJson["title"] = TCHAR_TO_UTF8("削峰填谷事件详情");
	ReturnJson["data"] = Datas;

	return ReturnJson;
}
Json OServerHelper::TheLatestPeakCuttingAndValleyFillingEvents(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EventInfo = MaraidbConnector.Query(str(boost::format("SELECT EventCode,EventType,StartDate,EndDate,TotolPlan,TotalComplete,EventStatus FROM VppEvent WHERE StartDate >='%1%' AND EndDate <='%2%' Order By StartDate Desc;") % StartTime % EndTime));

	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("事件编码") ,TCHAR_TO_UTF8("事件类型"),TCHAR_TO_UTF8("开始时间"),TCHAR_TO_UTF8("结束时间"),TCHAR_TO_UTF8("计划总量"),TCHAR_TO_UTF8("完成总量"),TCHAR_TO_UTF8("状态") };
	std::vector<std::string > RowWidth = { "15" ,"12","15","18","10","10","10" };

	Json Table = FillTableJson(RowHeader, RowWidth, EventInfo.DataTable);

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("最新削峰填谷事件");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}

Json OServerHelper::EB_TheLatestPeakCuttingAndValleyFillingEvents(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EventInfo = MaraidbConnector.Query(str(boost::format("SELECT EventCode,EventType,StartDate,EndDate,TotolPlan,TotalComplete FROM VppEvent WHERE StartDate >='%1%' AND EndDate <='%2%';") % StartTime % EndTime));

	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("事件编码"),TCHAR_TO_UTF8("事件类型"),TCHAR_TO_UTF8("开始时间"),TCHAR_TO_UTF8("结束时间"),TCHAR_TO_UTF8("计划总量"),TCHAR_TO_UTF8("完成总量") };
	std::vector<std::string > RowWidth = { "22.66" ,"10.66","22.66","22.66","10.66","10.66" };

	Json Table = FillTableJson(RowHeader, RowWidth, EventInfo.DataTable, 8);
	std::vector<Json> CurVector = { {{"value","XQXY201904280001"}},{{"value",TCHAR_TO_UTF8("削峰事件")}},{{"value","2019-04-27 13:00"}},{{"value","2019-04-28 14:00"}},{{"value",TCHAR_TO_UTF8("-----"}},{{"value","168.05"}} };
	Table["data"].insert(Table["data"].begin(), CurVector);
	std::vector<Json> Tables;
	//Tables.push_back({ {"value",""},{"value",""},{"value",""},{"value",""},{"value",""},{"value",""} });
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("削峰填谷事件回顾");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnergyUseTrendsThisMonth(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::vector<Json> TableDatas;

	boost::gregorian::date Now = boost::gregorian::day_clock::local_day();
	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '-', EXLabelStepDateIterator::Day);
	std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > boost::gregorian::to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig);

	{
		std::string ElecUseCommand = "SELECT MONTH(CreateDate),DAY(CreateDate),EnergyConsum FROM EnteElecInfoDay WHERE ElecMetID='" + ElecMetID + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' ORDER BY CreateDate;";
		FMysqlResult ElecUse = MaraidbConnector.Query(ElecUseCommand);

		std::vector<Json> ElecCategoryDatas;
		for (auto ElecUseRow : ElecUse.DataTable)
		{
			Json ElecCategoryData;
			ElecCategoryData["xLabel"] = ElecUseRow[0] + "-" + ElecUseRow[1];
			ElecCategoryData["yLabel"] = ElecUseRow[2];
			ElecCategoryDatas.push_back(ElecCategoryData);
		}

		Json ElecTableData;
		ElecTableData["categoryName"] = TCHAR_TO_UTF8("电");
		ElecTableData["start_color"] = "rgba(255,255,255,1)";
		ElecTableData["end_color"] = "rgba(255,255,255,1)";
		ElecTableData["high_color"] = "rgba(255,255,255,0.3)";
		ElecTableData["gradient_color"] = "rgba(255,255,255,0)";
		ElecTableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, ElecCategoryDatas);
		TableDatas.push_back(ElecTableData);
	}

	//{
	//	std::string WaterUseCommand = "SELECT MONTH(CreateDate),DAY(CreateDate),SUM(EnergyConsum) FROM EnteWaterInfo,EnteElec_EnteWater WHERE EnteElec_EnteWater.WaterMetID=EnteWaterInfo.WaterMetID AND EnteElec_EnteWater.ElecMetID='" + ElecMetID + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY CreateDate ORDER BY CreateDate;";
	//	FMysqlResult WaterUse = MaraidbConnector.Query(WaterUseCommand);

	//	std::vector<Json> WaterCategoryDatas;
	//	for ( auto WaterUseRow : WaterUse.DataTable )
	//	{
	//		Json WaterCategoryData;
	//		WaterCategoryData["xLabel"] = WaterUseRow[0] + "-" + WaterUseRow[1];
	//		WaterCategoryData["yLabel"] = WaterUseRow[2];
	//		WaterCategoryDatas.push_back(WaterCategoryData);
	//	}

	//	Json WaterTableData;
	//	WaterTableData["categoryName"] = TCHAR_TO_UTF8("水");
	//	WaterTableData["start_color"] = "rgba(48,165,255,1)";
	//	WaterTableData["end_color"] = "rgba(48,165,255,1)";
	//	WaterTableData["high_color"] = "rgba(48,165,255,0.3)";
	//	WaterTableData["gradient_color"] = "rgba(48,165,255,0)";
	//	WaterTableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime , WaterCategoryDatas);
	//	TableDatas.push_back(WaterTableData);
	//}
	//
	//std::string GasUseCommand = "SELECT MONTH(CreateDate),DAY(CreateDate),SUM(EnergyConsum) FROM EnteGasInfo,EnteGas_EnteElec WHERE EnteGas_EnteElec.GasMetID=EnteGasInfo.GasMetID AND EnteGas_EnteElec.ElecMetID='"+ ElecMetID +"' AND CreateDate BETWEEN '"+ StartTime +"' AND '"+ EndTime +"' GROUP BY CreateDate ORDER BY CreateDate;";
	//FMysqlResult GasUse = MaraidbConnector.Query(GasUseCommand);

	//std::vector<Json> GasCategoryDatas;
	//for ( auto GasUseRow : GasUse.DataTable )
	//{
	//	Json GasCategoryData;
	//	GasCategoryData["xLabel"] = GasUseRow[0] + "-" + GasUseRow[1];
	//	GasCategoryData["yLabel"] = GasUseRow[2];
	//	GasCategoryDatas.push_back(GasCategoryData);
	//}

	//Json GasTableData;
	//GasTableData["categoryName"] = TCHAR_TO_UTF8("气");
	//GasTableData["start_color"] = "rgba(8,227,211,1)";
	//GasTableData["end_color"] = "rgba(8,227,211,1)";
	//GasTableData["high_color"] = "rgba(8,227,211,0.3)";
	//GasTableData["gradient_color"] = "rgba(8,227,211,0)";
	//GasTableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime , GasCategoryDatas);
	//TableDatas.push_back(GasTableData);
	//
	std::vector<Json> Tables;
	Json Table;

	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = "(tce)";
	Table["xLabelRang"] = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("本月用能趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::EnergyConsumptionOfTheDay_RegionalAttributes(Json InData) {
	//std::string StartTime = InData["StartTime"];
	//std::string EndTime = InData["EndTime"];
	//std::string ElecMetID = InData["ElecMetID"];

	//std::string RequestAPI = HTTP_URL("/energyMonitor/getEnteAreaElec");
	//std::string RequsetData = "?begindate=" + StartTime + "&enddate=" + EndTime + "&elecNum=" + ElecMetID;
	//RequestAPI.append(RequsetData);
	//auto [ErrorCode , Status , Result] = cinatra::post(RequestAPI , "");
	//auto HttpReceiveInfo = Json::parse(Result);

	return {};

}
Json OServerHelper::IndustryUserClassification(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> EnteCountCategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		std::string IndustryEnteCountCommand = "SELECT COUNT(DISTINCT ElecMetID) FROM BaseEnteElecMeterInfo WHERE BaseEnteElecMeterInfo.IndustryID='" + ChildIndustry[0] + "'AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('" + RegionID + "'));";
		FMysqlResult IndustryEnteCount = MaraidbConnector.Query(IndustryEnteCountCommand);

		Json EnteCountCategoryData;
		EnteCountCategoryData["xLabel"] = ChildIndustry[1];
		EnteCountCategoryData["yLabel"] = IndustryEnteCount.DataTable[0][0];
		EnteCountCategoryDatas.push_back(EnteCountCategoryData);
	}
	std::sort(EnteCountCategoryDatas.begin(), EnteCountCategoryDatas.end(), SortCategoryDatas);

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("行业用户分布");
	ReturnData["data"] = EnteCountCategoryDatas;

	return ReturnData;
}
Json OServerHelper::EnergyMonitoring_EnterpriseProfile(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);

	std::string EnteIndustryCommand = "SELECT IndustryName FROM BaseIndustryInfo,BaseEnteElecMeterInfo WHERE BaseIndustryInfo.IndustryID=BaseEnteElecMeterInfo.IndustryID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';";
	FMysqlResult EnteIndustry = MaraidbConnector.Query(EnteIndustryCommand);

	std::string EnteRegionCommand = "SELECT RegionName FROM BaseRegionInfo,BaseEnteElecMeterInfo WHERE BaseRegionInfo.RegionID=BaseEnteElecMeterInfo.RegionID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';";
	FMysqlResult EnteRegion = MaraidbConnector.Query(EnteRegionCommand);

	std::string EnteInfoCommand = "SELECT VoltageLevel,RunCapacity,Address,EnteName FROM BaseEnteInfo,BaseEnteElecMeterInfo WHERE BaseEnteInfo.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "'";
	FMysqlResult EnteInfo = MaraidbConnector.Query(EnteInfoCommand);

	std::vector<Json> Table;
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"行业类型\", \"value\": \"----\" }")));
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"所属区域\", \"value\": \"----\" }")));
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"电压等级\", \"value\": \"----\" }")));
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"运行容量\", \"value\": \"----\" }")));
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"用电地址\", \"value\": \"----\" }")));

	for (auto EnteIndustryRow : EnteIndustry.DataTable)
		Table[0]["value"] = EnteIndustryRow[0];
	for (auto EnteRegionRow : EnteRegion.DataTable)
		Table[1]["value"] = EnteRegionRow[0];
	for (auto EnteInfoRow : EnteInfo.DataTable)
	{
		Table[2]["value"] = EnteInfoRow[0];
		Table[3]["value"] = EnteInfoRow[1] + "kVA";
		Table[4]["value"] = EnteInfoRow[2];
	}
	std::vector<Json> Datas;
	std::string Title = TCHAR_TO_UTF8("企业概况");
	if (EnteInfo.DataTable.size() > 0)
	{
		Title = EnteInfo.DataTable[0][3];
	}
	Json Data;
	Data["title"] = Title;
	Data["table"] = Table;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnergyAnalyze_EnterpriseProfile(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);

	std::string EnteIndustryCommand = "SELECT IndustryName FROM BaseIndustryInfo,BaseEnteElecMeterInfo WHERE BaseIndustryInfo.IndustryID=BaseEnteElecMeterInfo.IndustryID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';";
	FMysqlResult EnteIndustry = MaraidbConnector.Query(EnteIndustryCommand);

	std::string EnteRegionCommand = "SELECT RegionName FROM BaseRegionInfo,BaseEnteElecMeterInfo WHERE BaseRegionInfo.RegionID=BaseEnteElecMeterInfo.RegionID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';";
	FMysqlResult EnteRegion = MaraidbConnector.Query(EnteRegionCommand);

	std::string EnteInfoCommand = "SELECT VoltageLevel,JoinDate,EnteName FROM BaseEnteInfo,BaseEnteElecMeterInfo WHERE BaseEnteInfo.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "'";
	FMysqlResult EnteInfo = MaraidbConnector.Query(EnteInfoCommand);

	std::vector<Json> Table;
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"行业类型\", \"value\": \"----\" }")));
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"所属区域\", \"value\": \"----\" }"));
	Table.push_back(Json::parse(TCHAR_TO_UTF8("{\"name\": \"电压等级\", \"value\": \"----\" }")));
	Table.push_back("{\"name\": \"入驻日期\", \"value\": \"----\" }")));

	for (auto EnteIndustryRow : EnteIndustry.DataTable)
		Table[0]["value"] = EnteIndustryRow[0];
	for (auto EnteRegionRow : EnteRegion.DataTable)
		Table[1]["value"] = EnteRegionRow[0];
	for (auto EnteInfoRow : EnteInfo.DataTable)
	{
		Table[2]["value"] = EnteInfoRow[0];
		Table[3]["value"] = FillPlaceholderIfNull(EnteInfoRow[1],5);
	}
	std::vector<Json> Datas;
	std::string Title = TCHAR_TO_UTF8("企业概况");
	if (EnteInfo.DataTable.size() > 0)
	{
		Title = EnteInfo.DataTable[0][2];
	}
	Json Data;
	Data["title"] = Title;
	Data["table"] = Table;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::RegionUserClassification(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> EnteCountCategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(str(boost::format("CALL GetRegionInfo_EnteNum('%1%')") % ChildRegion[0]));

		Json EnteCountCategoryData;
		EnteCountCategoryData["xLabel"] = ChildRegion[1];
		EnteCountCategoryData["yLabel"] = RegionEnteCount.DataTable[0][0];
		EnteCountCategoryDatas.push_back(EnteCountCategoryData);
	}
	std::sort(EnteCountCategoryDatas.begin(), EnteCountCategoryDatas.end(), SortCategoryDatas);

	Json EnteCountTableData;
	EnteCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnteCountTableData["start_color"] = "rgba(209,246,255,1)";
	EnteCountTableData["end_color"] = "rgba(209,246,255,1)";
	EnteCountTableData["high_color"] = "#5ceaff";
	EnteCountTableData["categoryData"] = EnteCountCategoryDatas;

	std::vector<Json>EnteCountTableDatas;
	EnteCountTableDatas.push_back(EnteCountTableData);

	Json EnteCountTable;
	EnteCountTable["title"] = TCHAR_TO_UTF8("总");
	EnteCountTable["unit"] = TCHAR_TO_UTF8("家");
	EnteCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnteCountCategoryDatas);
	EnteCountTable["data"] = EnteCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnteCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("区域用户分布占比");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::EnergyMonitoring_RegionalOverview(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"];
	std::string EndTime = FIELDIFNULL(InData["EndTime"];

	FMysqlResult RegionArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(Area) FROM BaseEnteInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('%1%'));") % RegionID));
	double Area = boost::lexical_cast<double>(RegionArea.DataTable[0][0]) / 1000000;

	FMysqlResult RegionInfoResult = MaraidbConnector.Query(str(boost::format("SELECT RegionName,Area FROM BaseRegionInfo WHERE RegionID='%1%';") % RegionID));
	FMysqlResult RegionEnteCountResult = MaraidbConnector.Query(str(boost::format("CALL GetRegionInfo_EnteNum('%1%')") % RegionID));
	FMysqlResult RegionIndustryCountResult = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT IndustryID) FROM RegionIndustry WHERE FIND_IN_SET(RegionIndustry.RegionID,GetChildrenRegion('%1%'))") % RegionID));
	FMysqlResult RegionEnergyEfficiencyResult = MaraidbConnector.Query(str(boost::format("SELECT AVG(ClearEnergyRation),SUM(CarbonEmission),AVG(EnergyEcnoValueAdd),SUM(SynEnergyConsum),AVG(EnergyEcnoValueAdd),SUM(OutputValue),SUM(EnergyConsumUnitArea) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%'") % RegionID % StartTime % EndTime));


	Json ReturnData = {
		{"data",
			{
				{
					{"title",TCHAR_TO_UTF8("全区概况")},
						{"table",
							{
								{
									{{"name",TCHAR_TO_UTF8("所在区域")},{"unit",""},{"value",RegionInfoResult.DataTable[0][0]}},
									{{"name",TCHAR_TO_UTF8("行业数量")},{"unit",TCHAR_TO_UTF8("类")},{"value",RegionIndustryCountResult.DataTable[0][0]}},
									{{"name",TCHAR_TO_UTF8("接入企业数量")},{"unit",TCHAR_TO_UTF8("户")},{"value",RegionEnteCountResult.DataTable[0][0]}},
									{{"name",TCHAR_TO_UTF8("面积")},{"unit",TCHAR_TO_UTF8("km²")},{"value",SaveDecimalPlaces(RegionInfoResult.DataTable[0][1])}},
								},
								{
									{{"name",TCHAR_TO_UTF8("上月清洁能源消纳率")},{"unit","%"},{"value","100"}},
									{{"name",TCHAR_TO_UTF8("上月清洁能源占比")},{"unit","%"},{"value",SaveDecimalPlaces(RegionEnergyEfficiencyResult.DataTable[0][0])}},
									{{"name",TCHAR_TO_UTF8("上月碳排放")},{"unit",TCHAR_TO_UTF8("万t")},{"value",FillPlaceholderIfNull(ConvertToTenThousand(RegionEnergyEfficiencyResult.DataTable[0][1]))}},
									{{"name",TCHAR_TO_UTF8("上月经济增加值")},{"unit",TCHAR_TO_UTF8("万元")},{"value",FillPlaceholderIfNull(ConvertToTenThousand(RegionEnergyEfficiencyResult.DataTable[0][2]))}},
								},
								{
									{{"name",TCHAR_TO_UTF8("上月能耗总量")},{"unit",TCHAR_TO_UTF8("万tce")},{"value",FillPlaceholderIfNull(ConvertToTenThousand(RegionEnergyEfficiencyResult.DataTable[0][3]))}},
									{{"name",TCHAR_TO_UTF8("上月度电经济增加值")},{"unit",TCHAR_TO_UTF8("万元/kW·h")},{"value",FillPlaceholderIfNull(ConvertToTenThousand(RegionEnergyEfficiencyResult.DataTable[0][4]))}},
									{{"name",TCHAR_TO_UTF8("上月万元产值能耗")},{"unit",TCHAR_TO_UTF8("tce/万")},{"value",FillPlaceholderIfNull(SaveDecimalPlaces(RegionEnergyEfficiencyResult.DataTable[0][5]))}},
									{{"name",TCHAR_TO_UTF8("上月单位建筑面积能耗")},{"unit",TCHAR_TO_UTF8("tce/km²")},{"value",FillPlaceholderIfNull(ConvertToTenThousand(GetEnergyConsumptionUnitArea(RegionEnergyEfficiencyResult.DataTable[0][3],std::to_string(Area))))}},
								},

							}
						}
				}
			}
		}
	};




	return ReturnData;
}
Json OServerHelper::ComparisonChartOfSupplyandDemandForTheMonth(Json InData) {
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("当月供需对比图"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));
	TableTitle.push_back(TCHAR_TO_UTF8("电"));
	TableTitle.push_back(TCHAR_TO_UTF8("水"));
	TableTitle.push_back(TCHAR_TO_UTF8("气"));

	std::vector<std::string> Unit;
	Unit.push_back("(tce)");
	Unit.push_back(TCHAR_TO_UTF8("(kW·h)"));
	Unit.push_back(TCHAR_TO_UTF8("(m³)"));
	Unit.push_back(TCHAR_TO_UTF8("(m³)"));

	std::vector<std::string> CategoryName;
	CategoryName.push_back(TCHAR_TO_UTF8("供"));
	CategoryName.push_back(TCHAR_TO_UTF8("需"));

	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, false, true, true,
		'-', EXLabelStepDateIterator::Day));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(65,141,255,0)");
	StartColor.push_back("rgba(0,254,233,0)");

	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(65,141,255,1)");
	EndColor.push_back("rgba(0,254,233,1)");

	std::vector<std::string> HighColor;
	HighColor.push_back("#5ceaff");
	HighColor.push_back("#5ceaff");

	std::vector<std::string> GradientColor;
	GradientColor.push_back("#8FC31F");
	GradientColor.push_back("#00FBF2");

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionName = MaraidbConnector.Query(str(boost::format("SELECT RegionName FROM `BaseRegionInfo` WHERE RegionID='%1%';") % RegionID));

	std::string RegionEnergyEfficiencyCommand = "SELECT MONTH(CreateDate),DAY(CreateDate),SynEnergySupply,SynEnergyConsum,ElecSupplySelf,ElecConsumSelf,WaterSupplySelf,WaterConsumSelf,GasSupplySelf,GasConsumSelf FROM RegionEnergyEfficiencyDay WHERE RegionID='" + RegionID + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' ORDER BY CreateDate ;";
	FMysqlResult RegionEnergyEfficiency = MaraidbConnector.Query(RegionEnergyEfficiencyCommand);

	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{
		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{
			date Now = day_clock::local_day();
			std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);
			std::vector<Json>TableDatas;
			for (int CategoryIndex = 0; CategoryIndex < CategoryName.size(); CategoryIndex++)
			{
				Json TableData;
				TableData["categoryName"] = CategoryName[CategoryIndex % CategoryName.size()];
				TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
				TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
				TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];
				TableData["gradient_color"] = GradientColor[CategoryIndex % GradientColor.size()];

				std::vector<Json> CategoryDatas;
				for (auto RegionEnergyEfficiencyRow : RegionEnergyEfficiency.DataTable)
				{
					Json CategoryData;
					CategoryData["xLabel"] = RegionEnergyEfficiencyRow[0] + "-" + RegionEnergyEfficiencyRow[1];
					CategoryData["yLabel"] = RegionEnergyEfficiencyRow[(float)TableIndex * 2 + (float)CategoryIndex + 2];
					CategoryDatas.push_back(CategoryData);
				}
				TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
				TableDatas.push_back(TableData);
			}
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);
			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[TableIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
		}
		Json Data;
		Data["title"] = DataTitle[DataIndex] + "--" + (RegionName.ResultIsNotNull() ? RegionName.DataTable[0][0] : "");
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;

}
Json OServerHelper::IndustryConsumptionRankingOfTheMonth(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> xLabelRang;
	std::vector<Json> yLabelRang;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	auto result = MaraidbConnector.Query("SELECT IndustryID, SUM(SynEnergyConsum) AS num FROM MonthIndustryEnergyEfficiency WHERE STR_TO_DATE(CreateDate,'%Y-%m-%d') > DATE_SUB(CURDATE(), INTERVAL 1 MONTH) GROUP BY IndustryID ORDER BY num DESC");
	for (std::vector<std::string> resultData : result.DataTable) {
		auto result1 = MaraidbConnector.Query("SELECT IndustryName FROM baseindustryinfo WHERE IndustryID='" + resultData[0] + "'");
		for (std::vector<std::string> resultData1 : result1.DataTable) {
			Json list1;
			Json list2;
			Json list3;
			list1["label"] = resultData1[0];
			list3["xLabel"] = resultData1[0];
			list2["label"] = resultData[1];
			list3["yLabel"] = resultData[1];
			xLabelRang.push_back(list1);
			yLabelRang.push_back(list2);
			categoryData.push_back(list3);
		}
	}
	data1Json["categoryName"] = "aaa";//类别一
	data1Json["start_color"] = "rgba(209,246,255,1)";
	data1Json["end_color"] = "rgba(209,246,255,1)";
	data1Json["high_color"] = "#5ceaff";
	data1Json["categoryData"] = categoryData;
	data1.push_back(data1Json);
	tableJson["title"] = "xxx";//总
	tableJson["unit"] = "(tce)";
	tableJson["data"] = data1;
	tableJson["xLabelRang"] = xLabelRang;
	tableJson["yLabelRang"] = yLabelRang;
	table.push_back(tableJson);
	dataJson["title"] = "qqq";;//当月行业消费排名
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}
Json OServerHelper::PanoramicMonitoring_IndustryUserClassification(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	int num = 0;
	auto numResult = MaraidbConnector.Query("SELECT COUNT(*) FROM AccEnte_Industry");
	for (std::vector<std::string> numResultData : numResult.DataTable) {
		num = boost::lexical_cast<int>(numResultData[0]);
	}
	if (num == 0) {
		tableData["data"] = data;
	}
	else {
		auto res1 = MaraidbConnector.Query("SELECT IndustryID, COUNT(IndustryID) AS num FROM AccEnte_Industry GROUP BY IndustryID ORDER BY num DESC");
		for (std::vector<std::string> r1 : res1.DataTable) {
			Json list;
			auto res2 = MaraidbConnector.Query("SELECT IndustryName FROM BaseIndustryInfo WHERE IndustryID='" + r1[0] + "'");
			for (std::vector<std::string> r2 : res2.DataTable) {
				list["name"] = r2[0];
				list["value"] = boost::lexical_cast<std::string>(boost::lexical_cast<int>(r1[1]) * 100 / num) + "%";
				list["unit"] = TCHAR_TO_UTF8("kW·h");
			}
			data.push_back(list);
		}
		tableData["data"] = data;
	}
	tableData["title"] = "svds";//行业用户分布占比
	return tableData;
}
Json OServerHelper::PanoramicMonitoring_RegionUserClassification(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<Json> table;
	Json tableJson;
	std::vector<Json> data1;
	Json data1Json;
	std::vector<Json> categoryData;
	int num = 0;
	auto numResult = MaraidbConnector.Query("SELECT COUNT(*) FROM BaseEnteInfo");
	for (std::vector<std::string> numResultData : numResult.DataTable) {
		num = boost::lexical_cast<int>(numResultData[0]);
	}
	if (num == 0) {
		data1Json["data"] = categoryData;
	}
	else {
		auto res1 = MaraidbConnector.Query("SELECT RegionID, COUNT(RegionID) AS num FROM BaseEnteInfo GROUP BY RegionID ORDER BY num DESC");
		for (std::vector<std::string> r1 : res1.DataTable) {
			Json list;
			auto res2 = MaraidbConnector.Query("SELECT RegionName FROM BaseRegionInfo WHERE RegionID='" + r1[0] + "'");
			for (std::vector<std::string> r2 : res2.DataTable) {
				list["xLabel"] = r2[0];
				list["yLabel"] = boost::lexical_cast<std::string>(boost::lexical_cast<int>(r1[1]) * 100 / num);
			}
			categoryData.push_back(list);
		}
		data1Json["data"] = categoryData;
	}
	data1Json["categoryName"] = "csd";//需
	data1Json["start_color"] = "#8FC31F";
	data1Json["end_color"] = "#8FC31F";
	data1Json["high_color"] = "#5ceaff";
	data1Json["gradient_color"] = "#8FC31F5A";
	data1.push_back(data1Json);
	tableJson["title"] = "vwecs";//总
	tableJson["unit"] = "berber";//万tce
	tableJson["data"] = data1;
	table.push_back(tableJson);
	dataJson["title"] = "wbwewec";//区域用户分布占比
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	tableData["data"] = data;
	return tableData;
}
Json OServerHelper::PanoramicMonitoring_RegionalOverview(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string regionID = boost::lexical_cast<std::string>(FIELDIFNULL(InData["regionID"]));
	Json tableData;
	std::vector<Json> data;
	Json dataJson;
	std::vector<std::vector<Json>> table;
	std::vector<Json> table1;
	std::vector<Json> table2;
	std::vector<Json> table3;
	Json tableJson1_1;
	Json tableJson1_2;
	Json tableJson1_3;
	Json tableJson1_4;
	Json tableJson2_1;
	Json tableJson2_2;
	Json tableJson2_3;
	Json tableJson2_4;
	Json tableJson3_1;
	Json tableJson3_2;
	Json tableJson3_3;
	Json tableJson3_4;
	if (regionID != "") {
		auto areaResult = MaraidbConnector.Query("SELECT RegionName, Area FROM BaseRegionInfo WHERE RegionID=" + regionID);
		for (std::vector<std::string> areaArray : areaResult.DataTable) {
			tableJson1_1["name"] = "szqy";//所在区域
			tableJson1_1["value"] = areaArray[0];
			tableJson1_1["unit"] = "";
			tableJson1_4["name"] = "mj";//面积
			tableJson1_4["value"] = areaArray[1];
			tableJson1_4["unit"] = "km2";
		}
		auto regionResult = MaraidbConnector.Query("SELECT COUNT(*) FROM BaseEnteInfo WHERE RegionID=" + regionID);
		for (std::vector<std::string> regionResultData : regionResult.DataTable) {
			tableJson1_3["name"] = "jrqysl";//接入企业数量
			tableJson1_3["value"] = regionResultData[0];
			tableJson1_3["unit"] = "";
		}
		auto industryResult = MaraidbConnector.Query("SELECT COUNT(*) FROM AccEnte_Industry WHERE EnteID IN (SELECT EnteID FROM BaseEnteInfo WHERE RegionID=" + regionID + ")");
		for (std::vector<std::string> industryResultData : industryResult.DataTable) {
			tableJson1_2["name"] = "hysl";//行业数量
			tableJson1_2["value"] = industryResultData[0];
			tableJson1_2["unit"] = "";
		}
		auto energyResult = MaraidbConnector.Query("SELECT ClearEnergyRation, CarbonEmission, EnergyEcnoValueAddAdvanced, SynEnergyConsum, EnergyEcnoValueAdd, OutputValue, EnergyConsumUnitArea FROM MonthRegionEnergyEfficiency WHERE RegionID=" + regionID + " AND STR_TO_DATE(CreateDate,'%Y-%m-%d') > DATE_SUB(CURDATE(), INTERVAL 1 MONTH)");
		for (std::vector<std::string> energyResultData : energyResult.DataTable) {
			tableJson2_1["name"] = "dyqjnyxnl";//当月清洁能源消纳率
			tableJson2_1["value"] = "100%";
			tableJson2_1["unit"] = "";
			tableJson2_2["name"] = "dyqjnyzb";//当月清洁能源占比
			tableJson2_2["value"] = energyResultData[0];
			tableJson2_2["unit"] = "";
			tableJson2_3["name"] = "dytpf";//当月碳排放
			tableJson2_3["value"] = energyResultData[1];
			tableJson2_3["unit"] = "wd";//万吨
			tableJson2_4["name"] = "syjjzjz";//上月经济增加值
			float EnergyEcnoValueAddAdvanced = boost::lexical_cast<float>(energyResultData[2]);
			float EnergyEcnoValueAdd = boost::lexical_cast<float>(energyResultData[4]);
			tableJson2_4["value"] = boost::lexical_cast<std::string>(EnergyEcnoValueAdd / (EnergyEcnoValueAddAdvanced + 1));
			tableJson2_4["unit"] = "wy";//万元

			tableJson3_1["name"] = "dynhzl";//当月能耗总量
			tableJson3_1["value"] = energyResultData[3];
			tableJson1_1["unit"] = "wtce";//万tce
			tableJson3_2["name"] = "dyddjjzjz";//当月度电经济增加值
			tableJson3_2["value"] = energyResultData[4];
			tableJson3_2["unit"] = "w/kW·h";//万/kW·h
			tableJson3_3["name"] = "dywycznh";//当月万元产值能耗
			tableJson3_3["value"] = boost::lexical_cast<float>(energyResultData[5]) / boost::lexical_cast<float>(energyResultData[5]);
			tableJson3_3["unit"] = "tce/w";//tce/万
			tableJson3_2["name"] = "dydwjzmjnh";//当月单位建筑面积能耗
			tableJson3_2["value"] = energyResultData[6];
			tableJson3_2["unit"] = "tce/km²";
		}
	}
	table1.push_back(tableJson1_1);
	table1.push_back(tableJson1_2);
	table1.push_back(tableJson1_3);
	table1.push_back(tableJson1_4);
	table2.push_back(tableJson2_1);
	table2.push_back(tableJson2_2);
	table2.push_back(tableJson2_3);
	table2.push_back(tableJson2_4);
	table3.push_back(tableJson3_1);
	table3.push_back(tableJson3_2);
	table3.push_back(tableJson3_3);
	table3.push_back(tableJson3_4);
	table.push_back(table1);
	table.push_back(table2);
	table.push_back(table3);
	dataJson["title"] = "gvniesn";//全区概况
	dataJson["table"] = table;
	data.push_back(dataJson);
	tableData["data"] = data;
	return tableData;
}
Json OServerHelper::EnergyTrading_EnterpriseProfile(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string GoodsID = FIELDIFNULL(InData["GoodsID"]);
	std::string ReportOrderServiceListCommand = "SELECT EnteEvalvate.PrvdName,EnteEvalvate.ContractScore As StarNum,(100-EnteEvalvate.ContractScore) As NoStarNum,EnteEvalvate.EnteAddress As EnteAddress,EnteEvalvate.CommunicateAddress AS PostalAdddress,EnteEvalvate.ServiceScope\
												FROM EnteEvalvate, GoodsInfo\
												WHERE EnteEvalvate.EnteID = GoodsInfo.EnteID\
												AND GoodsInfo.GoodsID ='" + GoodsID + "';";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		RowData.push_back(Json::parse(TCHAR_TO_UTF8("{\"value\":\"企业简介\"}")));
		for (auto Value : ReportOrderServiceListRow)
		{
			Json CurValue;
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
		}
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("企业简介");
	return TableData;
}
Json OServerHelper::CommodityOrderQuantityCurve(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string GoodsID = FIELDIFNULL(InData["GoodsID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '-', EXLabelStepDateIterator::Month);

	std::vector<Json> XLabelRang = GetXLabelRang(StartTime, EndTime, Config);


	FMysqlResult Result = MaraidbConnector.Query(str(boost::format("SELECT YEAR(SignDate),MONTH(SignDate),COUNT(ContractId) FROM GoodsContractInfo WHERE GoodsID='%1%' AND SignDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(SignDate),MONTH(SignDate) ORDER BY SignDate ;") % GoodsID % StartTime % EndTime));

	std::vector<Json> CategoryDatas;
	for (auto ResultRow : Result.DataTable)
	{
		CategoryDatas.push_back({ {"xLabel",ResultRow[0] + "-" + ResultRow[1]},{"yLabel",ResultRow[2]} });
	}
	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("成交趋势");
	TableData["start_color"] = "rgba(209,246,255,1)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "rgba(209,246,255,0.3)";
	TableData["gradient_color"] = "rgba(209,246,255,0)";
	TableData["categoryData"] = FillCategoryData(XLabelRang, CategoryDatas);

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = XLabelRang;
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("商品订单数量曲线");
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ProductDetailsList(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"编号\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"名称\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"类型\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"企业名称\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"参考价\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"创建时间\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"商品评分（0-5）\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"状态\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"商品ID\",\"width\":\"464\"}")));


	std::vector<std::string> StatusList = { TCHAR_TO_UTF8("审批中"),TCHAR_TO_UTF8("已上架"),TCHAR_TO_UTF8("未通过"),TCHAR_TO_UTF8("已下架") };

	std::vector<Json> FinalData;
	for (auto GoodsStatus : StatusList)
	{
		std::string GoodStatusCountCommand = "SELECT COUNT(GoodsInfo.GoodsStatus) As Number FROM GoodsInfo\
										WHERE GoodsStatus = '" + GoodsStatus + "'AND GoodsInfo.CreateDate BETWEEN '" + StartTime + "' and '" + EndTime + "'";

		FMysqlResult GoodStatusCountList = MaraidbConnector.Query(GoodStatusCountCommand);

		Json CurDate;
		Json Title;

		Title["Name"] = GoodsStatus;
		Title["Value"] = GoodStatusCountList.DataTable[0][0];
		CurDate["title"] = Title;

		std::string GoodDetailsCommand = "SELECT GoodsInfo.GoodsCode,GoodsInfo.GoodsName,GoodsInfo.GoodsType,GoodsInfo.PrvdName,GoodsInfo.GoodsPrice,GoodsInfo.CreateDate,GoodsInfo.GoodsScore,GoodsStatus,GoodsInfo.GoodsID FROM GoodsInfo\
										WHERE GoodsStatus = '" + GoodsStatus + "'\
										AND GoodsInfo.CreateDate BETWEEN '" + StartTime + "' and '" + EndTime + "'\
										GROUP BY GoodsInfo.GoodsCode ;";

		FMysqlResult GoodDetailsCountList = MaraidbConnector.Query(GoodDetailsCommand);

		std::vector<Json> TableDatas;
		for (auto ReportOrderServiceListRow : GoodDetailsCountList.DataTable)
		{
			std::vector<Json> Datas;
			for (auto Value : ReportOrderServiceListRow)
			{
				Json Data;
				Data["value"] = Value;
				Datas.push_back(Data);
			}
			TableDatas.push_back(Datas);
		}
		while (TableDatas.size() < 13)
		{
			std::vector<Json> Datas;
			for (auto RowHeaderValue : RowHeader)
			{
				Json Data;
				Data["value"] = TCHAR_TO_UTF8("-----");
				Datas.push_back(Data);
			}
			TableDatas.push_back(Datas);
		}

		std::vector<Json> Tables;
		Json Table;
		Table["rowHeader"] = RowHeader;
		Table["data"] = TableDatas;
		Tables.push_back(Table);
		CurDate["table"] = Tables;
		FinalData.push_back(CurDate);
	}
	Json TableData;
	TableData["data"] = FinalData;
	return TableData;
}
Json OServerHelper::ProductInformation(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string GoodsID = FIELDIFNULL(InData["GoodsID"]);

	FMysqlResult GoodInfo = MaraidbConnector.Query(str(boost::format("SELECT GoodsName,GoodsType,GoodsPrice,ContactPerson,ContactPhone,GoodsDedtail,GoodsImgPath FROM GoodsInfo,EnteEvalvate WHERE GoodsInfo.GoodsID='%1%' AND GoodsInfo.EnteID=EnteEvalvate.EnteID;") % GoodsID));

	std::vector<Json> RowData;
	for (auto GoodInfoRow : GoodInfo.DataTable)
	{
		for (auto Value : GoodInfoRow)
		{
			Json CurValue;
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
		}
	}

	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("商品信息");
	return TableData;
}
Json OServerHelper::AbnormalEnterpriseProfile(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ExceptionEventId = FIELDIFNULL(InData["ExceptionEventId"]);

	FMysqlResult ElecMetIDResult = MaraidbConnector.Query(str(boost::format("SELECT ElecMetID FROM AbnormalEnergyEvent WHERE ExceptionEventId='%1%';") % ExceptionEventId));

	std::string ElecMetID;
	if (ElecMetIDResult.DataTable.size() > 0)
	{
		ElecMetID = ElecMetIDResult.DataTable[0][0];
	}

	if (ElecMetID == "")
	{
		return {};
	}

	std::string EnteInfoCommand = "SELECT BaseEnteInfo.EnteName,IFNULL(BaseIndustryInfo.IndustryName,''),BaseEnteInfo.VoltageLevel,BaseEnteInfo.JoinDate,BaseRegionInfo.RegionName,BaseEnteInfo.RunCapacity,BaseEnteInfo.Address FROM BaseEnteInfo,BaseRegionInfo,BaseEnteElecMeterInfo LEFT JOIN BaseIndustryInfo ON BaseEnteElecMeterInfo.IndustryID=BaseIndustryInfo.IndustryID WHERE BaseEnteElecMeterInfo.ElecMetID=BaseEnteInfo.ElecMetID AND BaseRegionInfo.RegionID=BaseEnteInfo.RegionID AND BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';";
	auto EnteInfo = MaraidbConnector.Query(EnteInfoCommand);

	if (!EnteInfo.ResultIsNotNull())return {};

	std::vector<Json> EnteLabels;

	std::vector<Json> Datas;
	Datas.push_back({ {"value",EnteInfo.DataTable[0][0] } });
	Datas.push_back({ { "value",EnteInfo.DataTable[0][1]} });
	Datas.push_back({ {"value",EnteInfo.DataTable[0][2] } });
	Datas.push_back({ { "value",FillPlaceholderIfNull(EnteInfo.DataTable[0][3],5)  } });
	Datas.push_back({ { "value",EnteInfo.DataTable[0][4] } });
	Datas.push_back({ { "value",EnteInfo.DataTable[0][5] + "kVA" } });
	Datas.push_back({ { "value",EnteInfo.DataTable[0][6] } });

	Json ReturnData;
	ReturnData["title"] = EnteInfo.DataTable[0][0];
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::DemandSide(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string DemandID = boost::lexical_cast<std::string>(FIELDIFNULL(InData["DemandID"]));
	Json tableData;
	Json data;

	if (DemandID != "") {
		auto areaResult = MaraidbConnector.Query("SELECT PrvID FROM DemandInfo WHERE DemandID=" + DemandID);
		for (std::vector<std::string> areaArray : areaResult.DataTable) {
			std::string sql2 = "SELECT PrvdName, ServiceScope FROM EnteEvalvate WHERE EnteID='" + areaArray[0] + "'GROUP BY PrvdName";
			auto result2 = MaraidbConnector.Query(sql2);
			for (std::vector<std::string> resultArray2 : result2.DataTable) {

				data["value1"] = resultArray2[0];
				data["value2"] = resultArray2[1];
			}
		}
	}
	tableData["title"] = TCHAR_TO_UTF8("供应方");
	tableData["data"] = data;
	return tableData;
}
Json OServerHelper::ListOfDetailedRequirements(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	//std::string RegionID = InData["RegionID"];
	std::string DemandStatusCountCommand = "SELECT DemandInfo.FlowStatus,COUNT(DemandInfo.FlowStatus) As Number FROM DemandInfo\
										WHERE DemandInfo.PublishDate BETWEEN '" + StartTime + "' and '" + EndTime + "'\
										GROUP BY DemandInfo.FlowStatus ";

	FMysqlResult DemandStatusCountList = MaraidbConnector.Query(DemandStatusCountCommand);

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"编号\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"名称\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"类型\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"企业名称\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"预算\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"意向金\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"发布时间\",\"width\":\"464\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"有效时间\",\"width\":\"464\"}")));
	std::vector<std::string> StatusList = { TCHAR_TO_UTF8("待付款"),TCHAR_TO_UTF8("审核中"),TCHAR_TO_UTF8("已发布"),TCHAR_TO_UTF8("未通过"),TCHAR_TO_UTF8("已关闭"),TCHAR_TO_UTF8("已发起合同") };

	std::vector<Json> FinalData;
	for (auto DemandStatusCountListRow : DemandStatusCountList.DataTable)
	{
		Json CurDate;
		Json Title;

		std::string DemandStatus = DemandStatusCountListRow[0];
		Title["Name"] = DemandStatus;
		Title["Value"] = DemandStatusCountListRow[1];
		CurDate["title"] = Title;

		if (StatusList.size() > 0)
		{
			std::vector <std::string>::iterator iElement = std::find(StatusList.begin(),
				StatusList.end(), DemandStatus);
			if (iElement != StatusList.end())
			{
				int nPosition = std::distance(StatusList.begin(), iElement);
				/*	std::cout << "Value  " << *iElement;
					std::cout << "  find in the vector at position: " << nPosition << std::endl;*/
				StatusList.erase(StatusList.begin() + nPosition);
			}
		}

		std::string DemandDetailsCommand = "SELECT DemandInfo.DemandCode,DemandInfo.DemandName,DemandInfo.DemandType,DemandInfo.EnteName,DemandInfo.Budget,DemandInfo.CashDeposit,DemandInfo.PublishDate,DemandInfo.VaildDate,DemandInfo.DemandID FROM DemandInfo\
										WHERE DemandInfo.FlowStatus = '" + DemandStatus + "'\
										AND DemandInfo.PublishDate BETWEEN '" + StartTime + "' and '" + EndTime + "'\
										GROUP BY DemandInfo.DemandCode ;";

		FMysqlResult DemandDetailsCountList = MaraidbConnector.Query(DemandDetailsCommand);

		std::vector<Json> TableDatas;
		for (auto ReportOrderServiceListRow : DemandDetailsCountList.DataTable)
		{
			std::vector<Json> Datas;
			for (auto Value : ReportOrderServiceListRow)
			{
				Json Data;
				Data["value"] = Value;
				Datas.push_back(Data);
			}
			TableDatas.push_back(Datas);
		}


		std::vector<Json> Tables;
		Json Table;
		Table["rowHeader"] = RowHeader;
		Table["data"] = TableDatas;
		Tables.push_back(Table);
		CurDate["table"] = Tables;
		FinalData.push_back(CurDate);
	}
	for (std::string CurStatus : StatusList)
	{
		Json CurDate;
		Json Title;
		std::vector<Json> Tables;
		Title["Name"] = CurStatus;
		Title["Value"] = 0;
		CurDate["title"] = Title;
		CurDate["table"] = Tables;
		FinalData.push_back(CurDate);
	}
	Json TableData;
	TableData["data"] = FinalData;
	return TableData;
}
Json OServerHelper::ShelvingDemandList(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"需求编号\",\"width\":\"14.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"需求名称\",\"width\":\"24.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"需求类型\",\"width\":\"10.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"发布时间\",\"width\":\"14.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"预算金额\",\"width\":\"12.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"发布企业\",\"width\":\"12.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"浏览次数\",\"width\":\"12.27\"}")));

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string DemandListCommand = TCHAR_TO_UTF8("SELECT DemandID,DemandName,DemandType,PublishDate,Budget,DemandInfo.EnteName,ViewNum FROM DemandInfo,BaseEnteInfo WHERE FlowStatus='审核中' AND BaseEnteInfo.EnteID=DemandInfo.EnteID AND FIND_IN_SET(BaseEnteInfo.RegionID,GetChildrenRegion('" + RegionID + "'))");
	FMysqlResult DemandList = MaraidbConnector.Query(DemandListCommand);

	std::vector<Json> TableDatas;
	for (auto DemandListRow : DemandList.DataTable)
	{
		std::vector<Json> TableData;
		for (auto Value : DemandListRow)
		{
			Json Filed;
			Filed["value"] = Value;
			TableData.push_back(Filed);
		}
		TableDatas.push_back(TableData);
	}
	while (TableDatas.size() < 8)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}
	std::vector<Json>Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("待审核需求列表");
	Data["table"] = Tables;
	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ProportionOfCustomerIndustryDistribution(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> EnteCountCategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		std::string IndustryEnteCountCommand = "SELECT COUNT(DISTINCT ElecMetID) FROM BaseEnteElecMeterInfo WHERE BaseEnteElecMeterInfo.IndustryID='" + ChildIndustry[0] + "'AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('" + RegionID + "'));";
		FMysqlResult IndustryEnteCount = MaraidbConnector.Query(IndustryEnteCountCommand);

		Json EnteCountCategoryData;
		EnteCountCategoryData["xLabel"] = ChildIndustry[1];
		EnteCountCategoryData["yLabel"] = IndustryEnteCount.DataTable[0][0];
		EnteCountCategoryDatas.push_back(EnteCountCategoryData);
	}
	std::sort(EnteCountCategoryDatas.begin(), EnteCountCategoryDatas.end(), SortCategoryDatas);
	FiltrationSmallDataByRatio(EnteCountCategoryDatas, 0.02f);

	Json EnteCountTableData;
	EnteCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnteCountTableData["start_color"] = "rgba(209,246,255,1)";
	EnteCountTableData["end_color"] = "rgba(209,246,255,1)";
	EnteCountTableData["high_color"] = "#5ceaff";
	EnteCountTableData["categoryData"] = EnteCountCategoryDatas;

	std::vector<Json>EnteCountTableDatas;
	EnteCountTableDatas.push_back(EnteCountTableData);

	Json EnteCountTable;
	EnteCountTable["title"] = TCHAR_TO_UTF8("总");
	EnteCountTable["unit"] = TCHAR_TO_UTF8("家");
	EnteCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnteCountCategoryDatas);
	EnteCountTable["data"] = EnteCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnteCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("客户行业分布占比");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::ES_ProportionOfCustomerIndustryDistribution(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> EnteCountCategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		std::string IndustryEnteCountCommand = "SELECT COUNT(DISTINCT ElecMetID) FROM BaseEnteElecMeterInfo WHERE BaseEnteElecMeterInfo.IndustryID='" + ChildIndustry[0] + "'AND FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('" + RegionID + "'));";
		FMysqlResult IndustryEnteCount = MaraidbConnector.Query(IndustryEnteCountCommand);

		Json EnteCountCategoryData;
		EnteCountCategoryData["xLabel"] = ChildIndustry[1];
		EnteCountCategoryData["yLabel"] = IndustryEnteCount.DataTable[0][0];
		EnteCountCategoryDatas.push_back(EnteCountCategoryData);
	}
	std::sort(EnteCountCategoryDatas.begin(), EnteCountCategoryDatas.end(), SortCategoryDatas);

	Json EnteCountTableData;
	EnteCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnteCountTableData["start_color"] = "rgba(96,199,255,0.7)";
	EnteCountTableData["end_color"] = "rgba(96,199,255,0.7)";
	EnteCountTableData["high_color"] = "#F2F2F2";
	EnteCountTableData["gradient_color"] = "#fce700";

	EnteCountTableData["categoryData"] = EnteCountCategoryDatas;

	std::vector<Json>EnteCountTableDatas;
	EnteCountTableDatas.push_back(EnteCountTableData);

	Json EnteCountTable;
	EnteCountTable["title"] = TCHAR_TO_UTF8("总");
	EnteCountTable["unit"] = TCHAR_TO_UTF8("家");
	EnteCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnteCountCategoryDatas);
	EnteCountTable["data"] = EnteCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnteCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("客户行业分布占比");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::CustomerAreaAnalysis(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> EnteCountCategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionEnteCountCommand = "SELECT COUNT(DISTINCT ElecMetID) FROM BaseEnteElecMeterInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + ChildRegion[0] + "'))";
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(RegionEnteCountCommand);

		Json EnteCountCategoryData;
		EnteCountCategoryData["xLabel"] = ChildRegion[1];
		EnteCountCategoryData["yLabel"] = RegionEnteCount.DataTable[0][0];
		EnteCountCategoryDatas.push_back(EnteCountCategoryData);
	}
	std::sort(EnteCountCategoryDatas.begin(), EnteCountCategoryDatas.end(), SortCategoryDatas);

	Json EnteCountTableData;
	EnteCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnteCountTableData["start_color"] = "rgba(209,246,255,1)";
	EnteCountTableData["end_color"] = "rgba(209,246,255,1)";
	EnteCountTableData["high_color"] = "#5ceaff";
	EnteCountTableData["categoryData"] = EnteCountCategoryDatas;

	std::vector<Json>EnteCountTableDatas;
	EnteCountTableDatas.push_back(EnteCountTableData);

	Json EnteCountTable;
	EnteCountTable["title"] = TCHAR_TO_UTF8("总");
	EnteCountTable["unit"] = TCHAR_TO_UTF8("家");
	EnteCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnteCountCategoryDatas);
	EnteCountTable["data"] = EnteCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnteCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("客户区域分布");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::ES_CustomerAreaAnalysis(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> EnteCountCategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionEnteCountCommand = "SELECT COUNT(DISTINCT ElecMetID) FROM BaseEnteElecMeterInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + ChildRegion[0] + "'))";
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(RegionEnteCountCommand);

		Json EnteCountCategoryData;
		EnteCountCategoryData["xLabel"] = ChildRegion[1];
		EnteCountCategoryData["yLabel"] = RegionEnteCount.DataTable[0][0];
		EnteCountCategoryDatas.push_back(EnteCountCategoryData);
	}
	std::sort(EnteCountCategoryDatas.begin(), EnteCountCategoryDatas.end(), SortCategoryDatas);

	Json EnteCountTableData;
	EnteCountTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnteCountTableData["start_color"] = "rgba(96,199,255,0.3)";
	EnteCountTableData["end_color"] = "rgba(96,199,255,1)";
	EnteCountTableData["high_color"] = "#5ceaff";
	EnteCountTableData["categoryData"] = EnteCountCategoryDatas;

	std::vector<Json>EnteCountTableDatas;
	EnteCountTableDatas.push_back(EnteCountTableData);

	Json EnteCountTable;
	EnteCountTable["title"] = TCHAR_TO_UTF8("总");
	EnteCountTable["unit"] = TCHAR_TO_UTF8("家");
	EnteCountTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnteCountCategoryDatas);
	EnteCountTable["data"] = EnteCountTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnteCountTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("客户区域分析");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::StarMerchandiseSales(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string GoodsSaleInfoCommand = "SELECT GoodsInfo.GoodsName,COUNT(GoodsContractInfo.GoodsID) FROM GoodsContractInfo,GoodsInfo WHERE GoodsInfo.GoodsID=GoodsContractInfo.GoodsID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsContractInfo.GoodsID ORDER BY COUNT(GoodsContractInfo.GoodsID) LIMIT 10;";
	FMysqlResult GoodsSaleInfo = MaraidbConnector.Query(GoodsSaleInfoCommand);

	std::vector<Json> CategoryDatas;
	for (auto GoodsSaleInfoRow : GoodsSaleInfo.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = GoodsSaleInfoRow[0];
		CategoryData["yLabel"] = GoodsSaleInfoRow[1];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.4)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("明星商品销量");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::ES_StarMerchandiseSales(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string GoodsSaleInfoCommand = "SELECT GoodsInfo.GoodsName,COUNT(GoodsContractInfo.GoodsID) FROM GoodsContractInfo,GoodsInfo WHERE GoodsInfo.GoodsID=GoodsContractInfo.GoodsID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsContractInfo.GoodsID ORDER BY COUNT(GoodsContractInfo.GoodsID) LIMIT 10;";
	FMysqlResult GoodsSaleInfo = MaraidbConnector.Query(GoodsSaleInfoCommand);

	std::vector<Json> CategoryDatas;
	for (auto GoodsSaleInfoRow : GoodsSaleInfo.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = GoodsSaleInfoRow[0];
		CategoryData["yLabel"] = GoodsSaleInfoRow[1];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("明星商品销量");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::ListOfEnergyServiceProviders(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();


	std::string EnteListCommand = "SELECT PrvdName,ServiceScope,ContractScore FROM EnteEvalvate;";
	FMysqlResult EnteList = MaraidbConnector.Query(EnteListCommand);

	std::vector<Json> TableDatas;
	for (auto EnteListRow : EnteList.DataTable)
	{
		Json TableData;
		TableData["name"] = EnteListRow[0];
		TableData["mainBusiness"] = EnteListRow[1];
		TableData["score"] = EnteListRow[2];
		std::string Star = "";
		std::string NoStar = TCHAR_TO_UTF8("★★★★★");

		if (TableData["score"] != "")
		{
			double StarNum = boost::lexical_cast<double>(EnteListRow[2]);
			StarNum /= 20;
			TableData["score"] = SaveDecimalPlaces(std::to_string(StarNum));
			StarNum = StarNum > 5 ? 5 : StarNum;
			for (int i = 0; i < StarNum; i++)
			{
				Star += TCHAR_TO_UTF8("★");
				boost::erase_last(NoStar, TCHAR_TO_UTF8("★"));
			}
		}
		TableData["star"] = Star;
		TableData["noStar"] = NoStar;

		TableDatas.push_back(TableData);
	}
	std::vector<Json> Tables;
	Json Table;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("能源服务商列表");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::RatingsAccounting(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	struct FScoreLevel
	{
		int MinScore = 0;
		int MaxScore = 5;
		std::string Des = "";
		FScoreLevel(int InMinScore = 0, int InMaxScore = 5, std::string InDes = "") :MinScore(InMinScore), MaxScore(InMaxScore), Des(InDes) { }
	};
	std::vector<FScoreLevel>ScoreLevels;
	ScoreLevels.push_back(FScoreLevel(0, 60, TCHAR_TO_UTF8("3分以下")));
	ScoreLevels.push_back(FScoreLevel(60, 70, "3.0-3.5"));
	ScoreLevels.push_back(FScoreLevel(70, 80, "3.5-4.0"));
	ScoreLevels.push_back(FScoreLevel(80, 90, "4.0-4.5"));
	ScoreLevels.push_back(FScoreLevel(90, 100, "4.5-5.0"));

	std::vector<Json> CategoryDatas;
	for (auto ScoreLevel : ScoreLevels)
	{
		std::string ScoreLevelCountCommand = "SELECT COUNT(DISTINCT EnteEvalvate.EnteID) FROM EnteEvalvate WHERE ContractScore >= '" + std::to_string(ScoreLevel.MinScore) + "' AND ContractScore< '" + std::to_string(ScoreLevel.MaxScore) + "' ;";
		FMysqlResult ScoreLevelCount = MaraidbConnector.Query(ScoreLevelCountCommand);

		if (ScoreLevelCount.DataTable.size() > 0)
		{
			Json CategoryData;
			CategoryData["xLabel"] = ScoreLevel.Des;
			CategoryData["yLabel"] = ScoreLevelCount.DataTable[0][0];
			CategoryDatas.push_back(CategoryData);
		}
	}
	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["end_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("评分占比");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::ES_RatingsAccounting(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	struct FScoreLevel
	{
		int MinScore = 0;
		int MaxScore = 5;
		std::string Des = "";
		FScoreLevel(int InMinScore = 0, int InMaxScore = 5, std::string InDes = "") :MinScore(InMinScore), MaxScore(InMaxScore), Des(InDes) { }
	};
	std::vector<FScoreLevel>ScoreLevels;
	ScoreLevels.push_back(FScoreLevel(0.0, 3.0, TCHAR_TO_UTF8("3分以下")));
	ScoreLevels.push_back(FScoreLevel(3.0, 3.5, "3.0-3.5"));
	ScoreLevels.push_back(FScoreLevel(3.5, 4.0, "3.5-4.0"));
	ScoreLevels.push_back(FScoreLevel(4.0, 4.5, "4.0-4.5"));
	ScoreLevels.push_back(FScoreLevel(4.5, 5.0, "4.5-5.0"));

	std::vector<Json> CategoryDatas;
	for (auto ScoreLevel : ScoreLevels)
	{
		std::string ScoreLevelCountCommand = "SELECT COUNT(DISTINCT EnteEvalvate.EnteID) FROM EnteEvalvate WHERE ContractScore BETWEEN '" + std::to_string(ScoreLevel.MinScore) + "' AND '" + std::to_string(ScoreLevel.MaxScore) + "' ;";
		FMysqlResult ScoreLevelCount = MaraidbConnector.Query(ScoreLevelCountCommand);

		if (ScoreLevelCount.DataTable.size() > 0)
		{
			Json CategoryData;
			CategoryData["xLabel"] = ScoreLevel.Des;
			CategoryData["yLabel"] = ScoreLevelCount.DataTable[0][0];
			CategoryDatas.push_back(CategoryData);
		}
	}
	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.6)";
	TableData["end_color"] = "rgba(96,199,255,0.6)";
	TableData["high_color1"] = "#F2F2F2";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = "%";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);

	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("产业联盟星级评定");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::ProportionOfCommodityTransactionCategories(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string GoodsSaleInfoCommand = "SELECT GoodsType,COUNT(GoodsContractInfo.GoodsID) FROM GoodsContractInfo,GoodsInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsType ORDER BY COUNT(GoodsContractInfo.GoodsID);";
	FMysqlResult GoodsSaleInfo = MaraidbConnector.Query(GoodsSaleInfoCommand);

	std::vector<Json> CategoryDatas;
	for (auto GoodsSaleInfoRow : GoodsSaleInfo.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = GoodsSaleInfoRow[0];
		CategoryData["yLabel"] = GoodsSaleInfoRow[1];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#00FDAD";
	TableData["end_color"] = "#00FDAD";
	TableData["high_color"] = "#00FDAD";
	TableData["gradient_color"] = "#00FDAD";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("商品成交分类占比");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::PercentageOfDemandTransactionCategories(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string DemmandCountCommand = "SELECT DemandType,COUNT(DemandContractInfo.DemandID) FROM DemandContractInfo,DemandInfo WHERE DemandContractInfo.DemandId=DemandInfo.DemandID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY DemandType ORDER BY COUNT(DemandContractInfo.DemandID);";
	FMysqlResult DemandCount = MaraidbConnector.Query(DemmandCountCommand);

	std::vector<Json> CategoryDatas;
	for (auto DemandCountRow : DemandCount.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = DemandCountRow[0];
		CategoryData["yLabel"] = DemandCountRow[1];
		CategoryDatas.push_back(CategoryData);
	}

	std::vector<Json> TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#00FDAD";
	TableData["end_color"] = "#00FDAD";
	TableData["high_color"] = "#00FDAD";
	TableData["gradient_color"] = "#00FDAD";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["data"] = TableDatas;
	Tables.push_back(Table);
	std::vector<Json>Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("需求成交分类占比");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

/*-------------------------------------------------------------------------------------------*/
Json OServerHelper::LabelClassification(Json InData) {

	struct FLabelGroupInfoHelper
	{
		static Json GetLabelClassification(std::string GroupID, std::string GroupCode, std::string GroupName)
		{
			FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
			OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
			MaraidbConnector.Connect();

			std::string LebelCountResultCommand = "SELECT COUNT(DISTINCT LabelID),LabelName FROM LabelInfo WHERE GroupID='" + GroupID + "';";
			FMysqlResult LabelCountResult = MaraidbConnector.Query(LebelCountResultCommand);

			std::string ChildrenLabelCommand = "SELECT LabelGroupInfo.GroupID,LabelGroupInfo.GroupCode,LabelGroupInfo.GroupName FROM LabelGroupInfo WHERE LabelGroupInfo.ParentCode='" + GroupCode + "';";
			FMysqlResult ChildrenLabel = MaraidbConnector.Query(ChildrenLabelCommand);
			std::vector<Json> Children;
			for (auto ChildrenLabelRow : ChildrenLabel.DataTable)
			{
				Children.push_back(GetLabelClassification(ChildrenLabelRow[0], ChildrenLabelRow[1], ChildrenLabelRow[2]));
			}
			if (boost::lexical_cast<int>(LabelCountResult.DataTable[0][0]) > 0)
			{
				Json Current;
				Current["name"] = LabelCountResult.DataTable[0][1];
				Current["value"] = boost::lexical_cast<int>(LabelCountResult.DataTable[0][0]);
				Children.push_back(Current);
			}
			Json ReturnData;
			ReturnData["name"] = GroupName;
			ReturnData["children"] = Children;
			return ReturnData;
		}
	};


	Json LabelClassification = FLabelGroupInfoHelper::GetLabelClassification("", "", "Root");
	Json ReturnJson;
	ReturnJson["data"] = LabelClassification["children"];

	return ReturnJson;
}

Json OServerHelper::LabelLifeCycle(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string Status[5] = { TCHAR_TO_UTF8("创建中"),TCHAR_TO_UTF8("发布中"),TCHAR_TO_UTF8("运行中"),TCHAR_TO_UTF8("停用"),TCHAR_TO_UTF8("待执行/暂停")};

	std::vector<Json> TableDatas;

	int Number[5] = { 0 };

	for (int i = 0; i < 5; i++)
	{
		FMysqlResult Count = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT LabelID) FROM LabelInfo WHERE LabelStatus='%1%';") % Status[i]));
		Number[i] = boost::lexical_cast<int>(Count.DataTable[0][0]);
	}

	for (int i = 0; i < 5; i++)
	{
		Json TableData;
		TableData["index"] = i;
		TableData["statusName"] = Status[i];
		if (i == 2)
		{
			TableData["value"] = Number[1] - Number[3];
		}
		else
		{
			TableData["value"] = Number[i];
		}
		TableDatas.push_back(TableData);
	}
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("标签全生命周期");
	Data["data"] = TableDatas;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::LabelOverview(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult LabelEnteCount = MaraidbConnector.Query("SELECT COUNT(DISTINCT EnteLabel.ElecMetID) FROM EnteLabel,BaseEnteInfo WHERE EnteLabel.ElecMetID=BaseEnteInfo.ElecMetID;");
	FMysqlResult EnteCount = MaraidbConnector.Query("SELECT COUNT(DISTINCT BaseEnteInfo.ElecMetID) FROM BaseEnteInfo;");

	FMysqlResult LabelCount = MaraidbConnector.Query("SELECT COUNT(DISTINCT LabelInfo.LabelID) FROM LabelInfo;");
	FMysqlResult RunLabelCount = MaraidbConnector.Query(TCHAR_TO_UTF8("SELECT COUNT(DISTINCT LabelInfo.LabelID) FROM LabelInfo WHERE LabelInfo.LabelStatus='发布中';"));

	Json TableData;
	std::vector<Json> Data;
	Json DataJson;
	std::vector<Json> Table;
	Json TableJson_1;
	Json TableJson_2;

	TableJson_1["currentLabel"] = TCHAR_TO_UTF8("标签客户");
	TableJson_1["fullLabel"] = TCHAR_TO_UTF8("客户总数");
	TableJson_1["currentValue"] = LabelEnteCount.DataTable[0][0];
	TableJson_1["fullValue"] = EnteCount.DataTable[0][0];

	TableJson_2["currentLabel"] = TCHAR_TO_UTF8("运行标签");
	TableJson_2["fullLabel"] = TCHAR_TO_UTF8("标签总数");
	TableJson_2["currentValue"] = RunLabelCount.DataTable[0][0];
	TableJson_2["fullValue"] = LabelCount.DataTable[0][0];

	Table.push_back(TableJson_1);
	Table.push_back(TableJson_2);

	DataJson["title"] = TCHAR_TO_UTF8("标签概况");
	DataJson["table"] = Table;

	Data.push_back(DataJson);
	TableData["data"] = Data;
	return TableData;
}

Json OServerHelper::EnergyConsumptionPerUnitAreaTrend(Json InData)
{

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	//std::string SeasonStartTime = InData["SeasonStartTime"];
	//std::string MonthStartTime = InData["MonthStartTime"];

	std::string EndTime = FIELDIFNULL(InData["EndTime"]);


	std::string Command_GetEnteMonthConsume = "Call GetEnteMonthConsumeUnitArea( '" + ElecMetID + "','" + YearStartTime + "','" + EndTime + "')";
	FMysqlResult EnteYearConsume = MaraidbConnector.Query(Command_GetEnteMonthConsume);

	//std::string Command_GetEnteSeasonDayConsume = "Call GetEnteDayConsumeUnitArea( '" + ElecMetID + "','" + SeasonStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteSeasonDayConsume = MaraidbConnector.Query(Command_GetEnteSeasonDayConsume);

	//std::string Command_GetEnteMonthDayConsume = "Call GetEnteDayConsumeUnitArea( '" + ElecMetID + "','" + MonthStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteMonthDayConsume = MaraidbConnector.Query(Command_GetEnteMonthDayConsume);

	std::vector<Json> Tables;

	{
		//Year
		Json YearTabelDataJson;
		YearTabelDataJson["categoryName"] = TCHAR_TO_UTF8("单位面积能耗");
		YearTabelDataJson["start_color"] = "rgba(65,141,255,0)";
		YearTabelDataJson["end_color"] = "rgba(65,141,255,1)";
		YearTabelDataJson["high_color"] = "#5ceaff";
		YearTabelDataJson["gradient_color"] = "#8FC31F";

		Json YearAdvanceTabelDataJson;
		YearAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
		YearAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
		YearAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
		YearAdvanceTabelDataJson["high_color"] = "#5ceaff";
		YearAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
		std::vector<Json> YearCategoryDatas;
		std::vector<Json> YearAdvanceCategoryDatas;

		FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '-', EXLabelStepDateIterator::Month);

		std::vector<Json> XLabelRang = GetXLabelRang(YearStartTime, EndTime, Config);

		//EnteYearConsume.Header
		//0:Year 
		//1:Month 
		//2:TCE
		//3:AdvancedTce 
		for (auto EnteYearConsumeRow : EnteYearConsume.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = EnteYearConsumeRow[0]+'-'+EnteYearConsumeRow[1];//Month
			CategoryData["yLabel"] = SaveDecimalPlaces(EnteYearConsumeRow[2], 4);//Tce
			YearCategoryDatas.push_back(CategoryData);

			Json AdvanceCategoryData;
			AdvanceCategoryData["xLabel"] = EnteYearConsumeRow[0] + '-'+EnteYearConsumeRow[1];//Month
			AdvanceCategoryData["yLabel"] = EnteYearConsumeRow[3];//AdvanceTce
			YearAdvanceCategoryDatas.push_back(AdvanceCategoryData);
		}
		YearTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearCategoryDatas);
		YearAdvanceTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearAdvanceCategoryDatas);
		std::vector<Json> YearTabelData;
		YearTabelData.push_back(YearTabelDataJson);
		YearTabelData.push_back(YearAdvanceTabelDataJson);

		Json YearTableJson;
		YearTableJson["title"] = TCHAR_TO_UTF8("年");
		YearTableJson["unit"] = TCHAR_TO_UTF8("月");
		YearTableJson["xLabelRang"] = XLabelRang;
		YearTableJson["data"] = YearTabelData;

		Tables.push_back(YearTableJson);
	}

	//{
	//	//Season
	//	Json SeasonTabelDataJson;
	//	SeasonTabelDataJson["categoryName"] = TCHAR_TO_UTF8("单位面积能耗");
	//	SeasonTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	SeasonTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	SeasonTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json SeasonAdvanceTabelDataJson;
	//	SeasonAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	SeasonAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	SeasonAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	SeasonAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> SeasonCategoryDatas;
	//	std::vector<Json> SeasonAdvanceCategoryDatas;

	//	//EnteSeasonDayConsume.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(3, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> SeasonXLabelRang = GetXLabelRang(SeasonStartTime, EndTime, Config);

	//	for (auto EnteSeasonConsumeRow : EnteSeasonDayConsume.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteSeasonConsumeRow[1] + "." + EnteSeasonConsumeRow[2];//Month.Day
	//		CategoryData["yLabel"] = SaveDecimalPlaces(EnteSeasonConsumeRow[3], 4);//Tce
	//		SeasonCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteSeasonConsumeRow[1] + "." + EnteSeasonConsumeRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = EnteSeasonConsumeRow[4];//AdvanceTce
	//		SeasonAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	SeasonTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonCategoryDatas);
	//	SeasonAdvanceTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonAdvanceCategoryDatas);


	//	std::vector<Json> SeasonTabelData;
	//	SeasonTabelData.push_back(SeasonTabelDataJson);
	//	SeasonTabelData.push_back(SeasonAdvanceTabelDataJson);

	//	Json SeasonTableJson;
	//	SeasonTableJson["title"] = TCHAR_TO_UTF8("季");
	//	SeasonTableJson["unit"] = "";
	//	SeasonTableJson["xLabelRang"] = SeasonXLabelRang;
	//	SeasonTableJson["data"] = SeasonTabelData;

	//	Tables.push_back(SeasonTableJson);
	//}

	//{
	//	//Month
	//	Json MonthTabelDataJson;
	//	MonthTabelDataJson["categoryName"] = TCHAR_TO_UTF8("单位面积能耗");
	//	MonthTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	MonthTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	MonthTabelDataJson["high_color"] = "#5ceaff";
	//	MonthTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json MonthAdvanceTabelDataJson;
	//	MonthAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	MonthAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	MonthAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	MonthAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	MonthAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> MonthCategoryDatas;
	//	std::vector<Json> MonthAdvanceCategoryDatas;

	//	//EnteMonthDayConsume.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> MonthXLabelRang = GetXLabelRang(MonthStartTime, EndTime, Config);

	//	for (auto EnteMonthConsumeRow : EnteMonthDayConsume.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteMonthConsumeRow[1] + "." + EnteMonthConsumeRow[2];//Month.Day
	//		CategoryData["yLabel"] = SaveDecimalPlaces(EnteMonthConsumeRow[3], 4);//Tce
	//		MonthCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteMonthConsumeRow[1] + "." + EnteMonthConsumeRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = EnteMonthConsumeRow[4];//AdvanceTce
	//		MonthAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	MonthTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthCategoryDatas);
	//	MonthAdvanceTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthAdvanceCategoryDatas);


	//	std::vector<Json> MonthTabelData;
	//	MonthTabelData.push_back(MonthTabelDataJson);
	//	MonthTabelData.push_back(MonthAdvanceTabelDataJson);

	//	Json MonthTableJson;
	//	MonthTableJson["title"] = TCHAR_TO_UTF8("天");
	//	MonthTableJson["unit"] = "";
	//	MonthTableJson["xLabelRang"] = MonthXLabelRang;
	//	MonthTableJson["data"] = MonthTabelData;

	//	Tables.push_back(MonthTableJson);
	//}
	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("单位面积能耗趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);
	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::EnteEcnoValueAddTrend(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	//std::string SeasonStartTime = InData["SeasonStartTime"];
	//std::string MonthStartTime = InData["MonthStartTime"];

	std::string EndTime = FIELDIFNULL(InData["EndTime"]);


	std::string Command_GetEnteMonthEcnoValueAdd = "Call GetEnteMonthEcnoValueAdd( '" + ElecMetID + "','" + YearStartTime + "','" + EndTime + "')";
	FMysqlResult EnteYearEcnoValueAdd = MaraidbConnector.Query(Command_GetEnteMonthEcnoValueAdd);

	//std::string Command_GetEnteSeasonDayEcnoValueAdd = "Call GetEnteDayEcnoValueAdd( '" + ElecMetID + "','" + SeasonStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteSeasonDayEcnoValueAdd = MaraidbConnector.Query(Command_GetEnteSeasonDayEcnoValueAdd);

	//std::string Command_GetEnteMonthDayEcnoValueAdd = "Call GetEnteDayEcnoValueAdd( '" + ElecMetID + "','" + MonthStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteMonthDayEcnoValueAdd = MaraidbConnector.Query(Command_GetEnteMonthDayEcnoValueAdd);

	std::vector<Json> Tables;

	{
		//Year
		Json YearTabelDataJson;
		YearTabelDataJson["categoryName"] = TCHAR_TO_UTF8("度电经济产值趋势");
		YearTabelDataJson["start_color"] = "rgba(65,141,255,0)";
		YearTabelDataJson["end_color"] = "rgba(65,141,255,1)";
		YearTabelDataJson["high_color"] = "#5ceaff";
		YearTabelDataJson["gradient_color"] = "#8FC31F";

		Json YearAdvanceTabelDataJson;
		YearAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
		YearAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
		YearAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
		YearAdvanceTabelDataJson["high_color"] = "#5ceaff";
		YearAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
		std::vector<Json> YearCategoryDatas;
		std::vector<Json> YearAdvanceCategoryDatas;

		FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '-', EXLabelStepDateIterator::Month);

		std::vector<Json> XLabelRang = GetXLabelRang(YearStartTime, EndTime, Config);

		//EnteYearEcnoValueAdd.Header
		//0:Year 
		//1:Month 
		//2:TCE
		//3:AdvancedTce 
		for (auto EnteYearEcnoValueAddRow : EnteYearEcnoValueAdd.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = EnteYearEcnoValueAddRow[0]+"-"+EnteYearEcnoValueAddRow[1];//Month
			CategoryData["yLabel"] = EnteYearEcnoValueAddRow[2];//Tce
			YearCategoryDatas.push_back(CategoryData);

			Json AdvanceCategoryData;
			AdvanceCategoryData["xLabel"] = EnteYearEcnoValueAddRow[0] + "-" + EnteYearEcnoValueAddRow[1];//Month
			AdvanceCategoryData["yLabel"] = EnteYearEcnoValueAddRow[3];//AdvanceTce
			YearAdvanceCategoryDatas.push_back(AdvanceCategoryData);
		}
		YearTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearCategoryDatas);
		YearAdvanceTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearAdvanceCategoryDatas);
		std::vector<Json> YearTabelData;
		YearTabelData.push_back(YearTabelDataJson);
		YearTabelData.push_back(YearAdvanceTabelDataJson);

		Json YearTableJson;
		YearTableJson["title"] = TCHAR_TO_UTF8("年");
		YearTableJson["unit"] = TCHAR_TO_UTF8("月");
		YearTableJson["xLabelRang"] = XLabelRang;
		YearTableJson["data"] = YearTabelData;

		Tables.push_back(YearTableJson);
	}

	//{
	//	//Season
	//	Json SeasonTabelDataJson;
	//	SeasonTabelDataJson["categoryName"] = TCHAR_TO_UTF8("度电经济产值趋势");
	//	SeasonTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	SeasonTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	SeasonTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json SeasonAdvanceTabelDataJson;
	//	SeasonAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	SeasonAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	SeasonAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	SeasonAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> SeasonCategoryDatas;
	//	std::vector<Json> SeasonAdvanceCategoryDatas;

	//	//EnteSeasonDayEcnoValueAdd.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(3, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> SeasonXLabelRang = GetXLabelRang(SeasonStartTime, EndTime, Config);

	//	for (auto EnteSeasonEcnoValueAddRow : EnteSeasonDayEcnoValueAdd.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteSeasonEcnoValueAddRow[1] + "." + EnteSeasonEcnoValueAddRow[2];//Month.Day
	//		CategoryData["yLabel"] = EnteSeasonEcnoValueAddRow[3];//Tce
	//		SeasonCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteSeasonEcnoValueAddRow[1] + "." + EnteSeasonEcnoValueAddRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = EnteSeasonEcnoValueAddRow[4];//AdvanceTce
	//		SeasonAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	SeasonTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonCategoryDatas);
	//	SeasonAdvanceTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonAdvanceCategoryDatas);


	//	std::vector<Json> SeasonTabelData;
	//	SeasonTabelData.push_back(SeasonTabelDataJson);
	//	SeasonTabelData.push_back(SeasonAdvanceTabelDataJson);

	//	Json SeasonTableJson;
	//	SeasonTableJson["title"] = TCHAR_TO_UTF8("季");
	//	SeasonTableJson["unit"] = "";
	//	SeasonTableJson["xLabelRang"] = SeasonXLabelRang;
	//	SeasonTableJson["data"] = SeasonTabelData;

	//	Tables.push_back(SeasonTableJson);
	//}

	//{
	//	//Month
	//	Json MonthTabelDataJson;
	//	MonthTabelDataJson["categoryName"] = TCHAR_TO_UTF8("度电经济产值趋势");
	//	MonthTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	MonthTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	MonthTabelDataJson["high_color"] = "#5ceaff";
	//	MonthTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json MonthAdvanceTabelDataJson;
	//	MonthAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	MonthAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	MonthAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	MonthAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	MonthAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> MonthCategoryDatas;
	//	std::vector<Json> MonthAdvanceCategoryDatas;

	//	//EnteMonthDayEcnoValueAdd.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> MonthXLabelRang = GetXLabelRang(MonthStartTime, EndTime, Config);

	//	for (auto EnteMonthEcnoValueAddRow : EnteMonthDayEcnoValueAdd.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteMonthEcnoValueAddRow[1] + "." + EnteMonthEcnoValueAddRow[2];//Month.Day
	//		CategoryData["yLabel"] = EnteMonthEcnoValueAddRow[3];//Tce
	//		MonthCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteMonthEcnoValueAddRow[1] + "." + EnteMonthEcnoValueAddRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = EnteMonthEcnoValueAddRow[4];//AdvanceTce
	//		MonthAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	MonthTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthCategoryDatas);
	//	MonthAdvanceTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthAdvanceCategoryDatas);


	//	std::vector<Json> MonthTabelData;
	//	MonthTabelData.push_back(MonthTabelDataJson);
	//	MonthTabelData.push_back(MonthAdvanceTabelDataJson);

	//	Json MonthTableJson;
	//	MonthTableJson["title"] = TCHAR_TO_UTF8("天");
	//	MonthTableJson["unit"] = "";
	//	MonthTableJson["xLabelRang"] = MonthXLabelRang;
	//	MonthTableJson["data"] = MonthTabelData;

	//	Tables.push_back(MonthTableJson);
	//}
	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("度电经济产值趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);
	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::EntePowerFactorTrend(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	//std::string SeasonStartTime = InData["SeasonStartTime"];
	//std::string MonthStartTime = InData["MonthStartTime"];

	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string Command_GetEnteMonthPowerFactor = "Call GetEnteMonthPowerFactor( '" + ElecMetID + "','" + YearStartTime + "','" + EndTime + "')";
	FMysqlResult EnteYearPowerFactor = MaraidbConnector.Query(Command_GetEnteMonthPowerFactor);

	//std::string Command_GetEnteSeasonDayPowerFactor = "Call GetEnteDayPowerFactor( '" + ElecMetID + "','" + SeasonStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteSeasonDayPowerFactor = MaraidbConnector.Query(Command_GetEnteSeasonDayPowerFactor);

	//std::string Command_GetEnteMonthDayPowerFactor = "Call GetEnteDayPowerFactor( '" + ElecMetID + "','" + MonthStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteMonthDayPowerFactor = MaraidbConnector.Query(Command_GetEnteMonthDayPowerFactor);

	std::vector<Json> Tables;

	{
		//Year
		Json YearTabelDataJson;
		YearTabelDataJson["categoryName"] = TCHAR_TO_UTF8("功率因数趋势");
		YearTabelDataJson["start_color"] = "rgba(65,141,255,0)";
		YearTabelDataJson["end_color"] = "rgba(65,141,255,1)";
		YearTabelDataJson["high_color"] = "#5ceaff";
		YearTabelDataJson["gradient_color"] = "#8FC31F";

		Json YearAdvanceTabelDataJson;
		YearAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
		YearAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
		YearAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
		YearAdvanceTabelDataJson["high_color"] = "#5ceaff";
		YearAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
		std::vector<Json> YearCategoryDatas;
		std::vector<Json> YearAdvanceCategoryDatas;

		FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '-', EXLabelStepDateIterator::Month);

		std::vector<Json> XLabelRang = GetXLabelRang(YearStartTime, EndTime, Config);

		//EnteYearPowerFactor.Header
		//0:Year 
		//1:Month 
		//2:TCE
		//3:AdvancedTce 
		for (auto EnteYearPowerFactorRow : EnteYearPowerFactor.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = EnteYearPowerFactorRow[0]+"-"+EnteYearPowerFactorRow[1];//Month
			CategoryData["yLabel"] = SaveDecimalPlaces(EnteYearPowerFactorRow[2], 2);//Tce
			YearCategoryDatas.push_back(CategoryData);

			Json AdvanceCategoryData;
			AdvanceCategoryData["xLabel"] = EnteYearPowerFactorRow[0] + "-" + EnteYearPowerFactorRow[1];//Month
			AdvanceCategoryData["yLabel"] = SaveDecimalPlaces(EnteYearPowerFactorRow[3], 2);//AdvanceTce
			YearAdvanceCategoryDatas.push_back(AdvanceCategoryData);
		}
		YearTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearCategoryDatas);
		YearAdvanceTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearAdvanceCategoryDatas);
		std::vector<Json> YearTabelData;
		YearTabelData.push_back(YearTabelDataJson);
		YearTabelData.push_back(YearAdvanceTabelDataJson);

		Json YearTableJson;
		YearTableJson["title"] = TCHAR_TO_UTF8("年");
		YearTableJson["unit"] = TCHAR_TO_UTF8("月");
		YearTableJson["xLabelRang"] = XLabelRang;
		YearTableJson["data"] = YearTabelData;

		Tables.push_back(YearTableJson);
	}

	//{
	//	//Season
	//	Json SeasonTabelDataJson;
	//	SeasonTabelDataJson["categoryName"] = TCHAR_TO_UTF8("功率因数趋势");
	//	SeasonTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	SeasonTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	SeasonTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json SeasonAdvanceTabelDataJson;
	//	SeasonAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	SeasonAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	SeasonAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	SeasonAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> SeasonCategoryDatas;
	//	std::vector<Json> SeasonAdvanceCategoryDatas;

	//	//EnteSeasonDayPowerFactor.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(3, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> SeasonXLabelRang = GetXLabelRang(SeasonStartTime, EndTime, Config);

	//	for (auto EnteSeasonPowerFactorRow : EnteSeasonDayPowerFactor.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteSeasonPowerFactorRow[1] + "." + EnteSeasonPowerFactorRow[2];//Month.Day
	//		CategoryData["yLabel"] = SaveDecimalPlaces(EnteSeasonPowerFactorRow[3], 2);//Tce
	//		SeasonCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteSeasonPowerFactorRow[1] + "." + EnteSeasonPowerFactorRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = SaveDecimalPlaces(EnteSeasonPowerFactorRow[4]);//AdvanceTce
	//		SeasonAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	SeasonTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonCategoryDatas);
	//	SeasonAdvanceTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonAdvanceCategoryDatas);

	//	std::vector<Json> SeasonTabelData;
	//	SeasonTabelData.push_back(SeasonTabelDataJson);
	//	SeasonTabelData.push_back(SeasonAdvanceTabelDataJson);

	//	Json SeasonTableJson;
	//	SeasonTableJson["title"] = TCHAR_TO_UTF8("季");
	//	SeasonTableJson["unit"] = "";
	//	SeasonTableJson["xLabelRang"] = SeasonXLabelRang;
	//	SeasonTableJson["data"] = SeasonTabelData;

	//	Tables.push_back(SeasonTableJson);
	//}

	//{
	//	//Month
	//	Json MonthTabelDataJson;
	//	MonthTabelDataJson["categoryName"] = TCHAR_TO_UTF8("功率因数趋势");
	//	MonthTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	MonthTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	MonthTabelDataJson["high_color"] = "#5ceaff";
	//	MonthTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json MonthAdvanceTabelDataJson;
	//	MonthAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	MonthAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	MonthAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	MonthAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	MonthAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> MonthCategoryDatas;
	//	std::vector<Json> MonthAdvanceCategoryDatas;

	//	//EnteMonthDayPowerFactor.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> MonthXLabelRang = GetXLabelRang(MonthStartTime, EndTime, Config);

	//	for (auto EnteMonthPowerFactorRow : EnteMonthDayPowerFactor.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteMonthPowerFactorRow[1] + "." + EnteMonthPowerFactorRow[2];//Month.Day
	//		CategoryData["yLabel"] = SaveDecimalPlaces(EnteMonthPowerFactorRow[3]);//Tce
	//		MonthCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteMonthPowerFactorRow[1] + "." + EnteMonthPowerFactorRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = SaveDecimalPlaces(EnteMonthPowerFactorRow[4]);//AdvanceTce
	//		MonthAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	MonthTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthCategoryDatas);
	//	MonthAdvanceTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthAdvanceCategoryDatas);


	//	std::vector<Json> MonthTabelData;
	//	MonthTabelData.push_back(MonthTabelDataJson);
	//	MonthTabelData.push_back(MonthAdvanceTabelDataJson);

	//	Json MonthTableJson;
	//	MonthTableJson["title"] = TCHAR_TO_UTF8("天");
	//	MonthTableJson["unit"] = "";
	//	MonthTableJson["xLabelRang"] = MonthXLabelRang;
	//	MonthTableJson["data"] = MonthTabelData;

	//	Tables.push_back(MonthTableJson);
	//}
	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("功率因数趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);
	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::CarbonEmissionsTrend(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	//std::string SeasonStartTime = InData["SeasonStartTime"];
	//std::string MonthStartTime = InData["MonthStartTime"];

	std::string EndTime = FIELDIFNULL(InData["EndTime"]);


	std::string Command_GetEnteMonthCarbonEmission = "Call GetEnteMonthCarbonEmission( '" + ElecMetID + "','" + YearStartTime + "','" + EndTime + "')";
	FMysqlResult EnteYearCarbonEmission = MaraidbConnector.Query(Command_GetEnteMonthCarbonEmission);

	//std::string Command_GetEnteSeasonDayCarbonEmission = "Call GetEnteDayCarbonEmission( '" + ElecMetID + "','" + SeasonStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteSeasonDayCarbonEmission = MaraidbConnector.Query(Command_GetEnteSeasonDayCarbonEmission);

	//std::string Command_GetEnteMonthDayCarbonEmission = "Call GetEnteDayCarbonEmission( '" + ElecMetID + "','" + MonthStartTime + "','" + EndTime + "')";
	//FMysqlResult EnteMonthDayCarbonEmission = MaraidbConnector.Query(Command_GetEnteMonthDayCarbonEmission);

	std::vector<Json> Tables;

	{
		//Year
		Json YearTabelDataJson;
		YearTabelDataJson["categoryName"] = TCHAR_TO_UTF8("碳排放趋势");
		YearTabelDataJson["start_color"] = "rgba(65,141,255,0)";
		YearTabelDataJson["end_color"] = "rgba(65,141,255,1)";
		YearTabelDataJson["high_color"] = "#5ceaff";
		YearTabelDataJson["gradient_color"] = "#8FC31F";

		Json YearAdvanceTabelDataJson;
		YearAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
		YearAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
		YearAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
		YearAdvanceTabelDataJson["high_color"] = "#5ceaff";
		YearAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
		std::vector<Json> YearCategoryDatas;
		std::vector<Json> YearAdvanceCategoryDatas;

		FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '-', EXLabelStepDateIterator::Month);

		std::vector<Json> XLabelRang = GetXLabelRang(YearStartTime, EndTime, Config);

		//EnteYearCarbonEmission.Header
		//0:Year 
		//1:Month 
		//2:TCE
		//3:AdvancedTce 
		for (auto EnteYearCarbonEmissionRow : EnteYearCarbonEmission.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = EnteYearCarbonEmissionRow[0]+"-"+EnteYearCarbonEmissionRow[1];//Month
			CategoryData["yLabel"] = SaveDecimalPlaces(EnteYearCarbonEmissionRow[2], 3);//Tce
			YearCategoryDatas.push_back(CategoryData);

			Json AdvanceCategoryData;
			AdvanceCategoryData["xLabel"] = EnteYearCarbonEmissionRow[0] + "-" + EnteYearCarbonEmissionRow[1];//Month
			AdvanceCategoryData["yLabel"] = EnteYearCarbonEmissionRow[3];//AdvanceTce
			YearAdvanceCategoryDatas.push_back(AdvanceCategoryData);
		}
		YearTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearCategoryDatas);
		YearAdvanceTabelDataJson["categoryData"] = FillCategoryData(XLabelRang, YearAdvanceCategoryDatas);
		std::vector<Json> YearTabelData;
		YearTabelData.push_back(YearTabelDataJson);
		YearTabelData.push_back(YearAdvanceTabelDataJson);

		Json YearTableJson;
		YearTableJson["title"] = TCHAR_TO_UTF8("年");
		YearTableJson["unit"] = TCHAR_TO_UTF8("月");
		YearTableJson["xLabelRang"] = XLabelRang;
		YearTableJson["data"] = YearTabelData;

		Tables.push_back(YearTableJson);
	}

	//{
	//	//Season
	//	Json SeasonTabelDataJson;
	//	SeasonTabelDataJson["categoryName"] = TCHAR_TO_UTF8("碳排放趋势");
	//	SeasonTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	SeasonTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	SeasonTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json SeasonAdvanceTabelDataJson;
	//	SeasonAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	SeasonAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	SeasonAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	SeasonAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	SeasonAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> SeasonCategoryDatas;
	//	std::vector<Json> SeasonAdvanceCategoryDatas;

	//	//EnteSeasonDayCarbonEmission.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(3, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> SeasonXLabelRang = GetXLabelRang(SeasonStartTime, EndTime, Config);

	//	for (auto EnteSeasonCarbonEmissionRow : EnteSeasonDayCarbonEmission.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteSeasonCarbonEmissionRow[1] + "." + EnteSeasonCarbonEmissionRow[2];//Month.Day
	//		CategoryData["yLabel"] = SaveDecimalPlaces(EnteSeasonCarbonEmissionRow[3], 3);//Tce
	//		SeasonCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteSeasonCarbonEmissionRow[1] + "." + EnteSeasonCarbonEmissionRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = EnteSeasonCarbonEmissionRow[4];//AdvanceTce
	//		SeasonAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	SeasonTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonCategoryDatas);
	//	SeasonAdvanceTabelDataJson["categoryData"] = FillCategoryData(SeasonXLabelRang, SeasonAdvanceCategoryDatas);


	//	std::vector<Json> SeasonTabelData;
	//	SeasonTabelData.push_back(SeasonTabelDataJson);
	//	SeasonTabelData.push_back(SeasonAdvanceTabelDataJson);

	//	Json SeasonTableJson;
	//	SeasonTableJson["title"] = TCHAR_TO_UTF8("季");
	//	SeasonTableJson["unit"] = "";
	//	SeasonTableJson["xLabelRang"] = SeasonXLabelRang;
	//	SeasonTableJson["data"] = SeasonTabelData;

	//	Tables.push_back(SeasonTableJson);
	//}

	//{
	//	//Month
	//	Json MonthTabelDataJson;
	//	MonthTabelDataJson["categoryName"] = TCHAR_TO_UTF8("碳排放趋势");
	//	MonthTabelDataJson["start_color"] = "rgba(65,141,255,0)";
	//	MonthTabelDataJson["end_color"] = "rgba(65,141,255,1)";
	//	MonthTabelDataJson["high_color"] = "#5ceaff";
	//	MonthTabelDataJson["gradient_color"] = "#8FC31F";

	//	Json MonthAdvanceTabelDataJson;
	//	MonthAdvanceTabelDataJson["categoryName"] = TCHAR_TO_UTF8("参考指标");
	//	MonthAdvanceTabelDataJson["start_color"] = "rgba(0,254,233,0)";
	//	MonthAdvanceTabelDataJson["end_color"] = "rgba(0,254,233,1)";
	//	MonthAdvanceTabelDataJson["high_color"] = "#5ceaff";
	//	MonthAdvanceTabelDataJson["gradient_color"] = "#00FBF2";
	//	std::vector<Json> MonthCategoryDatas;
	//	std::vector<Json> MonthAdvanceCategoryDatas;

	//	//EnteMonthDayCarbonEmission.Header
	//	//0:Year 
	//	//1:Month 
	//	//2:Day
	//	//3:Tce
	//	//4:AdvancedTce 
	//	FXLabelRangCofig Config = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
	//	std::vector<Json> MonthXLabelRang = GetXLabelRang(MonthStartTime, EndTime, Config);

	//	for (auto EnteMonthCarbonEmissionRow : EnteMonthDayCarbonEmission.DataTable)
	//	{
	//		Json CategoryData;
	//		CategoryData["xLabel"] = EnteMonthCarbonEmissionRow[1] + "." + EnteMonthCarbonEmissionRow[2];//Month.Day
	//		CategoryData["yLabel"] = SaveDecimalPlaces(EnteMonthCarbonEmissionRow[3], 3);//Tce
	//		MonthCategoryDatas.push_back(CategoryData);

	//		Json AdvanceCategoryData;
	//		AdvanceCategoryData["xLabel"] = EnteMonthCarbonEmissionRow[1] + "." + EnteMonthCarbonEmissionRow[2];//Month.Day
	//		AdvanceCategoryData["yLabel"] = EnteMonthCarbonEmissionRow[4];//AdvanceTce
	//		MonthAdvanceCategoryDatas.push_back(AdvanceCategoryData);
	//	}
	//	MonthTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthCategoryDatas);
	//	MonthAdvanceTabelDataJson["categoryData"] = FillCategoryData(MonthXLabelRang, MonthAdvanceCategoryDatas);


	//	std::vector<Json> MonthTabelData;
	//	MonthTabelData.push_back(MonthTabelDataJson);
	//	MonthTabelData.push_back(MonthAdvanceTabelDataJson);

	//	Json MonthTableJson;
	//	MonthTableJson["title"] = TCHAR_TO_UTF8("天");
	//	MonthTableJson["unit"] = "";
	//	MonthTableJson["xLabelRang"] = MonthXLabelRang;
	//	MonthTableJson["data"] = MonthTabelData;

	//	Tables.push_back(MonthTableJson);
	//}
	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("碳排放趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);
	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}

Json OServerHelper::EnterpriseComprehensiveEnergyConsumptionAnalysis(Json InData) {

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	if (ElecMetID == "")
	{
		return {};
	}
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string EnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption_SUM('" + ElecMetID + "','" + StartTime + "','" + EndTime + "')";
	FMysqlResult EnteEnergyConsumption = MaraidbConnector.Query(EnteEnergyConsumptionCommand);

	FMysqlResult EnteIndustryRegionInfo = MaraidbConnector.Query("SELECT IndustryID,RegionID FROM BaseEnteElecMeterInfo WHERE BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';");
	FMysqlResult IndustryRegionEnteCount = MaraidbConnector.Query("SELECT COUNT(ElecMetID) FROM BaseEnteElecMeterInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + EnteIndustryRegionInfo.DataTable[0][1] + "')) AND IndustryID='" + EnteIndustryRegionInfo.DataTable[0][0] + "';");

	std::string IndustryEnergyConsumptionCommand = "SELECT truncate(SUM(ElecConsum),2),truncate(SUM(WaterConsum),2),truncate(SUM(GasConsum),2) \
	FROM IndustryEnergyEfficiencyMonth \
	WHERE IndustryEnergyEfficiencyMonth.IndustryID='" + EnteIndustryRegionInfo.DataTable[0][0] + "' AND IndustryEnergyEfficiencyMonth.RegionID='" + EnteIndustryRegionInfo.DataTable[0][1] + "' \
	AND IndustryEnergyEfficiencyMonth.CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "'  ;";
	FMysqlResult IndustryEnergyConsumption = MaraidbConnector.Query(IndustryEnergyConsumptionCommand);

	std::vector<Json> TabelDatas;
	{
		Json IndustryTableData;
		IndustryTableData["categoryName"] = TCHAR_TO_UTF8("行业均值");
		IndustryTableData["start_color"] = "rgb(97,173,255)";
		IndustryTableData["end_color"] = "rgba(97,173,255,0.2)";
		IndustryTableData["high_color"] = "8EA7AD";
		IndustryTableData["gradient_color"] = "#445157";

		std::vector<Json> IndustryCategoryDatas;
		for (int i = 0; i < 3; i++)
		{
			Json IndustryCategoryData;
			IndustryCategoryData["yLabel"] = SaveDecimalPlaces(std::to_string(boost::lexical_cast<double>(IndustryEnergyConsumption.DataTable[0][i]) / boost::lexical_cast<int>(IndustryRegionEnteCount.DataTable[0][0])));
			IndustryCategoryDatas.push_back(IndustryCategoryData);
		}
		IndustryTableData["categoryData"] = IndustryCategoryDatas;
		TabelDatas.push_back(IndustryTableData);
	}
	{
		Json EnteTableData;
		EnteTableData["categoryName"] = TCHAR_TO_UTF8("企业");
		EnteTableData["start_color"] = "rgb(145,255,85)";
		EnteTableData["end_color"] = "rgba(145,255,85,0.2)";
		EnteTableData["high_color"] = "#6298E0";
		EnteTableData["gradient_color"] = "#223C58";

		std::vector<Json> EnteCategoryDatas;
		for (int i = 0; i < 3; i++)
		{
			Json EnteCategoryData;
			EnteCategoryData["yLabel"] = EnteEnergyConsumption.DataTable[0][i];
			EnteCategoryDatas.push_back(EnteCategoryData);
		}
		EnteTableData["categoryData"] = EnteCategoryDatas;
		TabelDatas.push_back(EnteTableData);
	}



	Json DataTable;
	DataTable["title"] = TCHAR_TO_UTF8("总");
	DataTable["unit"] = "Tce";

	std::vector<Json> XLabelRangs;
	for (int i = 0; i < 3; i++)
	{
		int MaxValue = 0;
		Json XLabelRang;
		switch (i)
		{
		case 0:
			XLabelRang["label"] = TCHAR_TO_UTF8("电");
			MaxValue = 10000;
			break;

		case 1:
			XLabelRang["label"] = TCHAR_TO_UTF8("水");
			MaxValue = 1000;
			break;
		case 2:
			XLabelRang["label"] = TCHAR_TO_UTF8("气");
			MaxValue = 100;
			break;
		default:
			break;
		}
		double IndustryConsumption = 0;
		if (IndustryEnergyConsumption.DataTable[0][i] != "")
		{
			IndustryConsumption = boost::lexical_cast<double>(IndustryEnergyConsumption.DataTable[0][i]);
		}
		double EnteConsumption = 0;
		if (EnteEnergyConsumption.DataTable[0][i] != "")
		{
			EnteConsumption = boost::lexical_cast<double>(EnteEnergyConsumption.DataTable[0][i]);
		}
		XLabelRang["maxValue"] = MaxValue;
		XLabelRangs.push_back(XLabelRang);
	}

	DataTable["xLabelRang"] = XLabelRangs;
	DataTable["data"] = TabelDatas;
	std::vector<Json> DataTables;
	DataTables.push_back(DataTable);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("企业能耗综合分析");
	Data["table"] = DataTables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;

	return TableData;
}

//实时接口
Json OServerHelper::EquipmentEnergyComparison(Json InData) {
	return "{ \"happy\": true, \"pi\": 3}"_json;
}
Json OServerHelper::MonthlyEquipmentEnergyUseAnalysis(Json InData) {
	return "{ \"happy\": true, \"pi\": 3.141}"_json;
}
Json OServerHelper::ComparisonOfComprehensiveEnergyConsumption(Json InData) {
	using namespace boost::gregorian;
	using namespace std;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	if (ElecMetID == "")
	{
		return {};
	}
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	date CurrentStartTime = from_string(StartTime);
	date CurrentEndTime = from_string(EndTime);

	date PreYearStartTime = CurrentStartTime - years(1);
	date PreYearEndTime = CurrentEndTime - years(1);

	FMysqlResult EnteIndustryRegionInfo = MaraidbConnector.Query("SELECT IndustryID,RegionID FROM BaseEnteElecMeterInfo WHERE BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';");
	FMysqlResult IndustryRegionEnteCount = MaraidbConnector.Query("SELECT COUNT(ElecMetID) FROM BaseEnteElecMeterInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + EnteIndustryRegionInfo.DataTable[0][1] + "')) AND IndustryID='" + EnteIndustryRegionInfo.DataTable[0][0] + "';");

	FMysqlResult CurrentElecInfoResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),Quantity,EnergyConsum FROM EnteElecInfo WHERE EnteElecInfo.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(CreateDate),MONTH(CreateDate) ORDER BY CreateDate DESC;") % ElecMetID % StartTime % EndTime));
	FMysqlResult CurrentWaterInfoResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),Volume,EnergyConsum FROM EnteWaterInfo,EnteElec_EnteWater WHERE EnteElec_EnteWater.WaterMetID=EnteWaterInfo.WaterMetID AND ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(CreateDate),MONTH(CreateDate) ORDER BY CreateDate DESC;") % ElecMetID % StartTime % EndTime));
	FMysqlResult CurrentGasInfoResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),Volume,EnergyConsum FROM EnteGasInfo,EnteGas_EnteElec WHERE EnteGasInfo.GasMetID=EnteGas_EnteElec.GasMetID AND ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(CreateDate),MONTH(CreateDate) ORDER BY CreateDate DESC;") % ElecMetID % StartTime % EndTime));

	FMysqlResult PreYearElecInfoResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),Quantity,EnergyConsum FROM EnteElecInfo WHERE EnteElecInfo.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(CreateDate),MONTH(CreateDate) ORDER BY CreateDate DESC;") % ElecMetID % to_iso_extended_string(PreYearStartTime) % to_iso_extended_string(PreYearEndTime)));
	FMysqlResult PreYearWaterInfoResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),Volume,EnergyConsum FROM EnteWaterInfo,EnteElec_EnteWater WHERE EnteElec_EnteWater.WaterMetID=EnteWaterInfo.WaterMetID AND ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(CreateDate),MONTH(CreateDate) ORDER BY CreateDate DESC;") % ElecMetID % to_iso_extended_string(PreYearStartTime) % to_iso_extended_string(PreYearEndTime)));
	FMysqlResult PreYearGasInfoResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),Volume,EnergyConsum FROM EnteGasInfo,EnteGas_EnteElec WHERE EnteGasInfo.GasMetID=EnteGas_EnteElec.GasMetID AND ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(CreateDate),MONTH(CreateDate) ORDER BY CreateDate DESC;") % ElecMetID % to_iso_extended_string(PreYearStartTime) % to_iso_extended_string(PreYearEndTime)));


	FMysqlResult ElecInfoResult[2] = { CurrentElecInfoResult,PreYearElecInfoResult };
	FMysqlResult WaterInfoResult[2] = { CurrentWaterInfoResult,PreYearWaterInfoResult };
	FMysqlResult GasInfoResult[2] = { CurrentGasInfoResult,PreYearGasInfoResult };


	std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气")};
	std::string Unit[4] = { "tce",TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³")};

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	date Now = day_clock::local_day();

	std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, to_iso_extended_string(CurrentEndTime > Now ? Now : CurrentEndTime), XLabelRangCofig);

	std::vector<Json> Tables;
	for (int i = 0; i < 4; i++)
	{
		std::vector<Json> TableDatas;
		for (int j = 0; j < 2; j++)
		{
			Json TableData;
			TableData["categoryName"] = j == 0 ? TCHAR_TO_UTF8("今年") : TCHAR_TO_UTF8("去年");
			TableData["start_color"] = j == 0 ? "rgba(97,173,255,0.2)" : "rgba(209,246,255,0.3)";
			TableData["end_color"] = j == 0 ? "rgb(97,173,255)" : "rgb(209,246,255)";
			TableData["high_color"] = "#5ceaff";
			TableData["gradient_color"] = "#00FBF2";

			std::vector<Json> CategoryDatas;

			if (i == 0)
			{
				std::vector<Json> ElecCategoryDatas;
				for (auto ElecInfoRow : ElecInfoResult[j].DataTable)
				{
					Json ElecCategoryData;
					ElecCategoryData["xLabel"] = ElecInfoRow[1];
					ElecCategoryData["yLabel"] = SaveDecimalPlaces(ElecInfoRow[3], 3);
					ElecCategoryDatas.push_back(ElecCategoryData);
				}

				std::vector<Json> WaterCategoryDatas;
				for (auto WaterInfoRow : WaterInfoResult[j].DataTable)
				{
					Json WaterCategoryData;
					WaterCategoryData["xLabel"] = WaterInfoRow[1];
					WaterCategoryData["yLabel"] = SaveDecimalPlaces(WaterInfoRow[3], 3);
					WaterCategoryDatas.push_back(WaterCategoryData);
				}

				std::vector<Json> GasCategoryDatas;
				for (auto GasInfoRow : GasInfoResult[j].DataTable)
				{
					Json GasCategoryData;
					GasCategoryData["xLabel"] = GasInfoRow[1];
					GasCategoryData["yLabel"] = SaveDecimalPlaces(GasInfoRow[3], 3);
					GasCategoryDatas.push_back(GasCategoryData);
				}
				std::vector<Json>CurrentXLabelRangs = j == 0 ? XLabelRangsCurrentTime : XLabelRangs;
				CategoryDatas = CombineCategoryData(CurrentXLabelRangs, GasCategoryDatas, CombineCategoryData(CurrentXLabelRangs, ElecCategoryDatas, WaterCategoryDatas));
			}
			else
			{
				FMysqlResult CurrentResult;
				switch (i)
				{
				case 1:
					CurrentResult = ElecInfoResult[j];
					break;
				case 2:
					CurrentResult = WaterInfoResult[j];
					break;

				case 3:
					CurrentResult = GasInfoResult[j];
					break;

				default:
					break;
				}
				for (auto CurrentResultInfoRow : CurrentResult.DataTable)
				{
					Json CategoryData;
					CategoryData["xLabel"] = CurrentResultInfoRow[1];
					CategoryData["yLabel"] = CurrentResultInfoRow[2];
					CategoryDatas.push_back(CategoryData);
				}
			}

			TableData["categoryData"] = FillCategoryData(j == 0 ? XLabelRangsCurrentTime : XLabelRangs, CategoryDatas);
			TableDatas.push_back(TableData);
		}
		Json Table;
		Table["title"] = Title[i];
		Table["unit"] = Unit[i];
		Table["xLabelRang"] = XLabelRangs;
		Table["data"] = TableDatas;

		Tables.push_back(Table);
	}

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("综合能耗趋势分析");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::AbnormalThisMonth(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string Status[2] = { TCHAR_TO_UTF8("未复检"),TCHAR_TO_UTF8("已确认")};

	std::vector<Json>Tables;
	int Count = 0;
	for (int i = 0; i < 2; i++)
	{
		FMysqlResult AbnormalEventsCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(ExceptionEventId) FROM AbnormalEnergyEvent,BaseEnteElecMeterInfo WHERE AbnormalEnergyEvent.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(RegionID,GetChildrenRegion('%1%')) AND FilterDate BETWEEN '%2%' AND '%3%' AND Status='%4%';") % RegionID % StartTime % EndTime % Status[i]));

		Json Table;
		Table["title"] = Status[i];
		Table["xLabel"] = TCHAR_TO_UTF8("个");
		Table["yLabel"] = AbnormalEventsCount.DataTable[0][0];
		Count += boost::lexical_cast<int>(AbnormalEventsCount.DataTable[0][0]);
		Tables.push_back(Table);
	}
	Json Table;
	Table["title"] = TCHAR_TO_UTF8("本月异常");
	Table["xLabel"] = TCHAR_TO_UTF8("个");
	Table["yLabel"] = std::to_string(Count);
	Tables.push_back(Table);

	Json Data;
	std::vector<Json> Datas;
	Data["title"] = TCHAR_TO_UTF8("本月异常事件");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
//待接口更新
Json OServerHelper::AbnormalEventDetails(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ExceptionEventId = FIELDIFNULL(InData["ExceptionEventId"]);
	FMysqlResult AbnormalEventsCount = MaraidbConnector.Query(str(boost::format("SELECT FilterDate,ReviewDate FROM AbnormalEnergyEvent WHERE ExceptionEventId='%1%';") % ExceptionEventId));

	if (AbnormalEventsCount.DataTable.size() <= 0)
	{
		return {};
	}

	Json Table = {
					{{"title",AbnormalEventsCount.DataTable[0][0]},{"xLabel",TCHAR_TO_UTF8("特征提取")},{"yLabel",""}},
					{{"title",AbnormalEventsCount.DataTable[0][0]},{"xLabel",TCHAR_TO_UTF8("异常辨识")},{"yLabel",""}},
					{{"title",AbnormalEventsCount.DataTable[0][0]},{"xLabel",TCHAR_TO_UTF8("原因预判")},{"yLabel",""}},
					{{"title",AbnormalEventsCount.DataTable[0][1]},{"xLabel",TCHAR_TO_UTF8("现场排查")},{"yLabel",""}},
	};
	Json Data;
	std::vector<Json> Datas;
	Data["title"] = TCHAR_TO_UTF8("异常事件详情");
	Data["table"] = Table;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::PredictAccuracyTrend(Json InData)
{
	using namespace cinatra;

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string RequestAPI = HTTP_URL("/aiException/getSelfLearning");
	std::string RequsetData = "?begindate=" + StartTime + "&enddate=" + EndTime;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	if (HttpReceiveInfo["code"] == "-1")
	{
		return {};
	}

	std::vector<Json> CategoryDatas;
	for (auto ReceiveData : HttpReceiveInfo["data"])
	{
		Json CategoryData;
		CategoryData["xLabel"] = ReceiveData["time"];
		CategoryData["yLabel"] = ReceiveData["precent"];
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("异常原因");
	TableData["start_color"] = "rgb(209,246,255)";
	TableData["end_color"] = "rgba(65,141,255,0.6)";
	TableData["high_color"] = "rgba(97,173,255,0.3)";
	TableData["gradient_color"] = "rgba(97,173,255,0.05)";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "%";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;

	Data["title"] = TCHAR_TO_UTF8("预判准确率趋势");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnterpriseLabel(Json InData) {

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = "";
	if (InData["ElecMetID"].is_null())
	{
		std::string ExceptionEventId = FIELDIFNULL(InData["ExceptionEventId"]);

		FMysqlResult ElecMetIDResult = MaraidbConnector.Query(str(boost::format("SELECT ElecMetID FROM AbnormalEnergyEvent WHERE ExceptionEventId='%1%';") % ExceptionEventId));
		if (ElecMetIDResult.DataTable.size() <= 0)
			return {};
		ElecMetID = ElecMetIDResult.DataTable[0][0];
	}
	else
	{
		ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	}

	std::string EnteLabelCommand = "SELECT DISTINCT EnteLabel.LabelName FROM EnteLabel WHERE EnteLabel.ElecMetID='" + ElecMetID + "';";
	FMysqlResult EnteLabel = MaraidbConnector.Query(EnteLabelCommand);

	std::vector<Json>Datas;
	for (auto EnteLabelRow : EnteLabel.DataTable)
	{
		Json Data;
		Data["value"] = EnteLabelRow[0];
		Datas.push_back(Data);
	}

	Json TableData;
	TableData["title"] = TCHAR_TO_UTF8("企业标签");
	TableData["data"] = Datas;

	return TableData;
}
Json OServerHelper::ReportOrderServiceList(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ApplyStatus = FIELDIFNULL(InData["ApplyStatus"]);
	std::string StrStatus;
	if (ApplyStatus == "")
		StrStatus = "";
	else
		StrStatus = "AND ServerApply.OrderStatus = '" + ApplyStatus + "'";
	std::string ReportOrderServiceListCommand = "SELECT ServerApply.OrderCode, ServerApply.OrderName, BaseEnteInfo.EnteName, ServerApply.OrderMoney, ServerApply.OrderDate FROM ServerApply, BaseEnteElecMeterInfo,BaseEnteInfo\
												WHERE ServerApply.EnteID = BaseEnteElecMeterInfo.EnteID\
												AND ServerApply.EnteID = BaseEnteInfo.EnteID\
												" + StrStatus + "\
												GROUP BY ServerApply.OrderDate\
												ORDER BY ServerApply.OrderDate DESC;";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;



	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"订单编号\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"服务名称\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"企业\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"订单金额\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"发布时间\",\"width\":\"20\"}")_json);



	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 4)
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);
	}

	while (TableDatas.size() < 9)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("报告订单服务列表");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}

Json OServerHelper::CustomizedService_IndustryComparison(Json InData) {

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult ChildIndustryList = MaraidbConnector.Query("SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE BaseIndustryInfo.ParentID='' OR ISNULL(BaseIndustryInfo.ParentID);");

	std::vector<Json> CategoryDatas;

	for (auto ChildIndustry : ChildIndustryList.DataTable)
	{
		FMysqlResult Count = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT ApplyID) FROM CustApply,BaseEnteElecMeterInfo WHERE CustApply.PayTime BETWEEN '%1%' AND '%2%' AND ApplyStatus='编制完成' AND ApplyEnteID=EnteID AND IndustryID ='%3%'")) % StartTime % EndTime % ChildIndustry[0]));

		Json CategoryData;
		CategoryData["xLabel"] = ChildIndustry[1];
		CategoryData["yLabel"] = Count.DataTable[0][0];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.25)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("个性化定制服务-行业对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::CustomizedServiceList(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ApplyStatus = FIELDIFNULL(InData["ApplyStatus"]);
	std::string StrStatus;
	if (ApplyStatus == "")
		StrStatus = "";
	else
		StrStatus = "WHERE CustApply.ApplyStatus = '" + ApplyStatus + "'";
	std::string ReportOrderServiceListCommand = "SELECT CustApply.ApplyCode, CustApply.ApplyServiceName, CustApply.ApplyEnteName, CustApply.BudgetPrice, CustApply.CreateDate FROM CustApply\
												" + StrStatus + "\
												GROUP BY CustApply.CreateDate\
												ORDER BY CustApply.CreateDate DESC;";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;



	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"订单编号\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"服务名称\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"企业\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"预算\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"发布时间\",\"width\":\"20\"}")_json);



	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 4)
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);
	}


	while (TableDatas.size() < 9)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("个性化定制服务列表");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::CustomizedService_RegionalComparison(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> CategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		std::string RegionEnteCountCommand = "SELECT COUNT(DISTINCT ApplyID) FROM CustApply,BaseEnteInfo WHERE CustApply.PayTime BETWEEN '" + StartTime + "' AND '" + EndTime + TCHAR_TO_UTF8("' AND ApplyEnteID=EnteID AND ApplyStatus='编制完成' AND FIND_IN_SET(RegionID,GetChildrenRegion('") + ChildRegion[0] + "'));";
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(RegionEnteCountCommand);

		Json CategoryData;
		CategoryData["xLabel"] = ChildRegion[1];
		CategoryData["yLabel"] = RegionEnteCount.DataTable[0][0];
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatasReverse);

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(209,246,255,0.25)";
	TableData["end_color"] = "rgba(209,246,255,1)";
	TableData["high_color"] = "#5ceaff";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("个性化定制服务-区域对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::CustomizedServiceDetail(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);
	std::string ReportOrderServiceListCommand = "SELECT CustApply.ApplyServiceName, CustApply.ApplyEnteName, BaseEnteInfo.Address, CustApply.OfferPrice, CustApply.CreateDate, CustApply.RequirementDes, CustApply.ApplyCode, CustApply.ContactPerson, CustApply.ContactPhone, CustApply.PayTime, CustApply.CloseDate\
												FROM CustApply, BaseEnteInfo\
												WHERE CustApply.ApplyEnteID = BaseEnteInfo.EnteID\
												AND CustApply.ApplyCode ='" + ApplyCode + "';";
	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;
	std::string FirstHalfTitle;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		for (auto Value : ReportOrderServiceListRow)
		{
			Json CurValue;
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
		}
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("XX公司（上海分公司）综合能耗监控定制服务");
	return TableData;
}
Json OServerHelper::CustomizedService_MonthlyConsumption(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult Result = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT (@i:=@i+1)i,ApplyEnteName,Money FROM (select @i:=0) AS A,(SELECT ApplyEnteName,SUM(OfferPrice) AS Money FROM CustApply  WHERE ApplyStatus='编制完成' AND PayTime BETWEEN '%1%' AND '%2%' GROUP BY ApplyEnteName ORDER BY SUM(OfferPrice)DESC) AS T;")) % StartTime % EndTime));

	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("排名"),TCHAR_TO_UTF8("企业名称"),TCHAR_TO_UTF8("月消费")};
	std::vector<std::string > RowWidth = { "10","10","10" };

	Json Table = FillTableJson(RowHeader, RowWidth, Result.DataTable,8);
	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("个性化定制服务消费月排行");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

//数据图表存在问题
Json OServerHelper::AccessServiceDetail(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);
	std::string ReportOrderServiceListCommand = "SELECT AccApply.ApplyAccName,AccApply.ApplyEnteName,BaseEnteInfo.Address,AccApply.OfferPrice,AccApply.CreateDate,AccApply.RequirementDes,AccApply.ApplyCode,AccApply.ContactPerson,AccApply.ContactPhone,AccApply.PayTime,AccApply.CloseDate\
												FROM AccApply,BaseEnteInfo\
												WHERE AccApply.ApplyEnteID = BaseEnteInfo.EnteID\
												AND AccApply.ApplyCode ='" + ApplyCode + "';";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		for (auto Value : ReportOrderServiceListRow)
		{
			Json CurValue;
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
		}
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("综合能耗监控接入服务-套餐1：户号接入");
	return TableData;
}
Json OServerHelper::AccessDetail(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string ApplyCode = FIELDIFNULL(InData["ApplyCode"]);
	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(str(boost::format("SELECT  AccPlace.EnteShortName, BaseIndustryInfo.IndustryName, AccPlace.Address, AccPlace.Area, AccPlace.ElecMetID, AccPlace.WaterMetID, AccPlace.GasMetID FROM BaseIndustryInfo, AccPlace WHERE AccPlace.ApplyCode in(SELECT ApplyCode FROM AccApply WHERE AccApply.ApplyEnteID = (SELECT ApplyEnteID From AccApply WHERE AccApply.ApplyCode = '%1%')) AND BaseIndustryInfo.IndustryID = AccPlace.IndustryGbCode") % ApplyCode));
	std::vector<Json> RowHeader;
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"站点\",\"width\":\"14.27\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"所属行业\",\"width\":\"10.27\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"详细地址\",\"width\":\"24.27\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"建筑面积(㎡)\",\"width\":\"14.27\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"电系户号\",\"width\":\"12.27\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"水销根号\",\"width\":\"12.27\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"水销根号\",\"width\":\"12.27\"}")_json);



	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		for (auto Value : ReportOrderServiceListRow)
		{

			Json Data;
			Data["value"] = Value;
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}


	while (TableDatas.size() < 4)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("接入详情");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::AccessServiceList(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ApplyStatus = FIELDIFNULL(InData["ApplyStatus"]);
	std::string StrStatus;
	if (ApplyStatus == "")
		StrStatus = "";
	else
		StrStatus = " WHERE AccApply.ApplyStatus = '" + ApplyStatus + "'";
	std::string ReportOrderServiceListCommand = "SELECT AccApply.ApplyCode,AccApply.ApplyAccName,AccApply.ApplyEnteName,AccApply.OfferPrice,AccApply.CreateDate FROM AccApply,BaseEnteElecMeterInfo \
												" + StrStatus + "\
												GROUP BY AccApply.CreateDate\
												ORDER BY AccApply.CreateDate DESC";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);

	std::vector<Json> RowHeader;

	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"订单编号\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"服务名称\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"企业\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"报价金额\",\"width\":\"20\"}")_json);
	RowHeader.push_back(TCHAR_TO_UTF8("{\"headerName\":\"发布时间\",\"width\":\"20\"}")_json);


	std::vector<Json> TableDatas;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		std::vector<Json> Datas;
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json Data;
			if (Index == 4)
			{
				boost::posix_time::ptime CurTime = boost::posix_time::time_from_string(Value);
				Data["value"] = boost::gregorian::to_iso_extended_string(CurTime.date());
			}
			else
				Data["value"] = Value;
			Datas.push_back(Data);
			Index++;
		}
		TableDatas.push_back(Datas);

	}

	while (TableDatas.size() < 9)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("数据接入服务列表");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}

//实时接口
Json OServerHelper::LoadCurve(Json InData) {
	using namespace cinatra;
	using namespace  boost::posix_time;
	using namespace  boost::gregorian;

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);

	ptime XLabelTime = time_from_string(StartTime + " 00:00:00");

	std::string RequestAPI = HTTP_URL("/energyManage/fullycovered/load");
	std::string RequsetData = "?date=" + StartTime;
	RequestAPI += RequsetData;
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> CategoryDatas;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		float TimeIndex = boost::lexical_cast<float>(DataRow["timeIndex"].is_null()?"0": DataRow["timeIndex"].get<std::string>());
		std::string CurrentTime = to_simple_string((XLabelTime + hours((int)TimeIndex)).time_of_day());
		CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
		CategoryDatas.push_back({ {"xLabel",CurrentTime},{"yLabel",std::to_string(FIELDIFNULL_FLOAT(DataRow["load"]))} });
	}

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("实时负载");
	TableData["lineStyle"] = "solid";
	TableData["start_color"] = "RGB(209,246,255)";
	TableData["end_color"] = "RGBA(65,141,255,0.6)";
	TableData["high_color"] = "RGBA(97,173,255,0.3)";
	TableData["gradient_color"] = "RGBA(97,173,255,0.05)";
	TableData["categoryData"] = CategoryDatas;
	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "kW";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("实时全口径负荷");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}


Json OServerHelper::RefusalToInviteEnterprises(Json InData) {
	return "{ \"happy\": false, \"pi\": 4}"_json;
}
Json OServerHelper::EnterpriseOutputCurve_table_PurePicture(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::EnterpriseOutputCurve_PurePicture(Json InData) {
	return "{ \"happy\": false, \"pi\": 4}"_json;
}
Json OServerHelper::EnterpriseOutputCurve_title(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::EnterpriseOutputCurve(Json InData) {
	return "{ \"happy\": false, \"pi\": 4}"_json;
}
Json OServerHelper::EnterpriseOutputCurve_BoundVersion_End(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::EnterpriseOutputCurve_BoundVersion_Execution(Json InData) {
	return "{ \"happy\": false, \"pi\": 4}"_json;
}
Json OServerHelper::ShanghaiGeneralHeavyIndustryGroupCompany(Json InData) {
	using namespace cinatra;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string DemandID = FIELDIFNULL(InData["DemandID"]);
	std::string BeginDate = FIELDIFNULL(InData["BeginDate"]);
	std::string EndDate = FIELDIFNULL(InData["EndDate"]);
	std::string ReportOrderServiceListCommand = "SELECT DemandInfo.EnteName, DemandInfo.ServiceScope,DemandInfo.DemandDesc FROM DemandInfo WHERE DemandInfo.DemandID='" + DemandID + "'";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;


	std::string RequestAPI = HTTP_URL("/energyTrading/getDemandList");
	std::string RequsetData = "?begindate=" + BeginDate + "&enddate=" + EndDate;
	RequestAPI += RequsetData;
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);
	Json EnteLabelDatas;
	if (HttpReceiveInfo["code"] != "-1")
	{
		std::vector<std::vector<std::string>>InsertTable;

		auto Date = boost::gregorian::to_iso_extended_string(boost::gregorian::day_clock::local_day());

		EnteLabelDatas = HttpReceiveInfo["data"];

	}
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json CurValue;
			if (Index == 0)
			{
				CurValue["value"] = TCHAR_TO_UTF8("需求方-") + Value;
			}
			else if (Index == 2)
			{
				CurValue["value"] = TCHAR_TO_UTF8("暂无");
				RowData.push_back(CurValue);
				CurValue["value"] = Value;
				RowData.push_back(CurValue);
			}
			else
			{
				CurValue["value"] = Value;
			}

			RowData.push_back(CurValue);

			Index++;
		}
		for (auto EnteLabelData : EnteLabelDatas)
		{
			Json CurValue;
			if (EnteLabelData["demandId"] == DemandID)
			{
				for (auto CurEnteLabelData : EnteLabelData["recommandPrvdList"])
				{
					CurValue["value"] = CurEnteLabelData;
					RowData.push_back(CurValue);
				}
				break;
			}
		}
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("企业简介");
	return TableData;
}
Json OServerHelper::ReviewOfEnterpriseContribution_End(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::LineRealTimeSituation(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult Result = MaraidbConnector.Query("SELECT LowLoadLineCount,MidLoadLineCount,HighLoadLineCount FROM LineLoadCache;");

	Json HighLoadCategoryData;
	HighLoadCategoryData["start_color"] = "rgb(255,213,176)";
	HighLoadCategoryData["high_color"] = "rgba(255,132,0,0.6)";
	HighLoadCategoryData["xLabel"] = TCHAR_TO_UTF8("过载线路");
	HighLoadCategoryData["yLabel"] = Result.DataTable[0][2];

	Json MidLoadCategoryData;
	MidLoadCategoryData["start_color"] = "rgb(145,255,85)";
	MidLoadCategoryData["high_color"] = "rgba(145,255,85,0.6)";
	MidLoadCategoryData["xLabel"] = TCHAR_TO_UTF8("重载线路");
	MidLoadCategoryData["yLabel"] = Result.DataTable[0][1];

	Json LowLoadCategoryData;
	LowLoadCategoryData["start_color"] = "rgb(209,246,255)";
	LowLoadCategoryData["Low_color"] = "rgba(65,141,255,0.6)";
	LowLoadCategoryData["xLabel"] = TCHAR_TO_UTF8("中载线路");
	LowLoadCategoryData["yLabel"] = Result.DataTable[0][0];

	std::vector<Json>CategoryDatas;
	CategoryDatas.push_back(LowLoadCategoryData);
	CategoryDatas.push_back(MidLoadCategoryData);
	CategoryDatas.push_back(HighLoadCategoryData);

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#00FDAD";
	TableData["end_color"] = "#00FDAD";
	TableData["high_color"] = "#00FDAD";
	TableData["gradient_color"] = "#00FDAD";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = TCHAR_TO_UTF8("条");
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("线路实时态势");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::VirtualPowerPlantOutputCurve(Json InData) {
	return "{ \"happy\": false, \"pi\": 4}"_json;
}
Json OServerHelper::MapOfPeakCutandValleyFilling(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult Result = MaraidbConnector.Query("SELECT BaseRegionInfo.RegionName,COUNT(DistributionFacilties.RegionID) FROM LineLoad,DistributionFacilties,BaseRegionInfo\
													WHERE LineLoad.LoadRate > 50\
													AND  LineLoad.StatElecID = DistributionFacilties.StatElecID\
													AND  BaseRegionInfo.RegionID  = DistributionFacilties.RegionID\
													AND BaseRegionInfo.ParentID='zj'\
													GROUP BY BaseRegionInfo.RegionName");
	std::vector<Json> DataArray;
	std::vector<std::string> RegionArray = { TCHAR_TO_UTF8("原国家高新区"),TCHAR_TO_UTF8("孙桥科创中心"),TCHAR_TO_UTF8("张江南区"),TCHAR_TO_UTF8("规划保留用地"),TCHAR_TO_UTF8("康桥工业园北区"),TCHAR_TO_UTF8("上海国际医学园区"),TCHAR_TO_UTF8("康桥工业园南区")};

	for (auto CurResult : Result.DataTable)
	{
		Json Data;
		Data["name"] = CurResult[0];
		Data["value"] = CurResult[1];
		auto iter = find(RegionArray.begin(), RegionArray.end(), CurResult[0]);
		if (iter!= RegionArray.end())RegionArray.erase(iter);
		DataArray.push_back(Data);
	}
	for (auto CurResult : RegionArray)
	{
		Json Data;
		Data["name"] = CurResult;
		Data["value"] = 0;
		DataArray.push_back(Data);
	}
	Json ReturnData;
	ReturnData["data"] = DataArray;
	return ReturnData;
}
Json OServerHelper::OverviewOfPeakCuttingAndValleyFillingEvents(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::ExecutionProgress(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::EnergyConsumptionOfTheDay_DeviceAttributes(Json InData) {
	return "{ \"happy\": false, \"pi\": 3}"_json;
}
Json OServerHelper::EnterpriseEnergyConsumptionStructure(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult ElecEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteElecInfo WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
	FMysqlResult WaterEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteWaterInfo,EnteElec_EnteWater WHERE EnteElec_EnteWater.WaterMetID=EnteWaterInfo.WaterMetID AND EnteElec_EnteWater.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%'") % ElecMetID % StartTime % EndTime));
	FMysqlResult GasEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteGasInfo,EnteGas_EnteElec WHERE EnteGas_EnteElec.GasMetID=EnteGasInfo.GasMetID AND EnteGas_EnteElec.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%'") % ElecMetID % StartTime % EndTime));


	std::vector<Json> CategoryDatas;
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("电")},{"yLabel",ElecEnergyConsumResult.DataTable[0][0]} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("水")},{"yLabel",WaterEnergyConsumResult.DataTable[0][0]} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("气")},{"yLabel",GasEnergyConsumResult.DataTable[0][0]} });

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color1"] = "#5ceaff";
	TableData["high_color2"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = "tce";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("企业上月用能结构");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;

}

Json OServerHelper::EnergyAnalyze_EnterpriseEnergyConsumptionStructure(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult ElecEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteElecInfo WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
	FMysqlResult WaterEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteWaterInfo,EnteElec_EnteWater WHERE EnteElec_EnteWater.WaterMetID=EnteWaterInfo.WaterMetID AND EnteElec_EnteWater.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%'") % ElecMetID % StartTime % EndTime));
	FMysqlResult GasEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteGasInfo,EnteGas_EnteElec WHERE EnteGas_EnteElec.GasMetID=EnteGasInfo.GasMetID AND EnteGas_EnteElec.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%'") % ElecMetID % StartTime % EndTime));

	std::vector<Json> CategoryDatas;
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("电")},{"yLabel",ElecEnergyConsumResult.DataTable[0][0]} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("水")},{"yLabel",WaterEnergyConsumResult.DataTable[0][0]} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("气")},{"yLabel",GasEnergyConsumResult.DataTable[0][0]} });

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color1"] = "#5ceaff";
	TableData["high_color2"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = "tce";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("企业月度用能结构");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::KeyMonitoringOfEnergyConsumptionInTheMonth(Json InData) {
	using namespace boost::gregorian;
	using namespace std;

	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("当月能源消费重点监测"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));
	std::vector<std::string> Unit;
	Unit.push_back("(tce)");
	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, false, false, true,
		'-', EXLabelStepDateIterator::Day));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(37,189,239,0.5)");
	StartColor.push_back("rgba(39,143,255,0.5)");
	StartColor.push_back("rgba(0,215,229,0.5)");

	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(37,189,239,1)");
	EndColor.push_back("rgba(39,143,255,1)");
	EndColor.push_back("rgba(0,215,229,0.5)");

	std::vector<std::string> HighColor;
	HighColor.push_back("rgba(37,189,239,1)");
	HighColor.push_back("rgba(39,143,255,1)");
	HighColor.push_back("rgba(0,215,229,0.5)");

	std::vector<std::string> GradientColor;
	GradientColor.push_back("rgba(37,189,239,1)");
	GradientColor.push_back("rgba(39,143,255,1)");
	GradientColor.push_back("rgba(0,215,229,0.5)");


	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string TopCount = FIELDIFNULL(InData["TopCount"]);

	FMysqlResult RegionName = MaraidbConnector.Query(str(boost::format("SELECT RegionName FROM `BaseRegionInfo` WHERE RegionID='%1%';") % RegionID));

	std::string EnteTopListCommand = "CALL GetEnteTopListByEnergyConsumption('" + RegionID + "','" + StartTime + "','" + EndTime + "','" + TopCount + "')";
	FMysqlResult EnteTopList = MaraidbConnector.Query(EnteTopListCommand);

	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{
		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{
			date Now = day_clock::local_day();
			std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			std::vector<Json>TableDatas;
			int CategoryIndex = 0;
			for (auto EnteTopListRow : EnteTopList.DataTable)
			{
				Json TableData;
				TableData["categoryName"] = TCHAR_TO_UTF8("企业");
				TableData["categoryName"] = EnteTopListRow[1];
				TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
				TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
				TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];
				TableData["gradient_color"] = GradientColor[CategoryIndex % GradientColor.size()];

				std::string EnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption_Day('" + EnteTopList.DataTable[CategoryIndex][0] + "','" + StartTime + "','" + EndTime + "')";
				FMysqlResult EnteEnergyConsumption = MaraidbConnector.Query(EnteEnergyConsumptionCommand);
				std::vector<Json> CategoryDatas;
				for (auto EnteEnergyConsumptionRow : EnteEnergyConsumption.DataTable)
				{
					boost::gregorian::date Date = boost::gregorian::from_string(EnteEnergyConsumptionRow[0]);
					Json CategoryData;
					CategoryData["xLabel"] = std::to_string(Date.day());
					CategoryData["yLabel"] = EnteEnergyConsumptionRow[(float)TableIndex + 1];
					CategoryDatas.push_back(CategoryData);
				}
				TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
				TableDatas.push_back(TableData);
				CategoryIndex++;
			}
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[TableIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
		}
		Json Data;
		Data["title"] = DataTitle[DataIndex] + "--" + (RegionName.ResultIsNotNull() ? RegionName.DataTable[0][0] : "");
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;
}
Json OServerHelper::ContractEvaluation(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string DemandID = FIELDIFNULL(InData["DemandID"]);
	std::string ReportOrderServiceListCommand = "SELECT ContractScoreSpeed,ContractScoreQuality,ContractScoreAttitude,ContractEvalDesc FROM DemandContractInfo\
												WHERE DemandContractInfo.DemandId = '" + DemandID + "';";

	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		int Index = 0;
		for (auto Value : ReportOrderServiceListRow)
		{
			Json CurValue;
			if (Index <= 2)
			{
				bool IsNumber = true;
				for (int i = 0; i < Value.size(); i++)
				{
					int tmp = (int)Value[i];
					if ((tmp >= 48 && tmp <= 57) || tmp == 46)
					{
						continue;
					}
					else
					{
						IsNumber = false;
						break;
					}
				}
				if (IsNumber)
				{
					double CurNumber = 0;
					if (Value != "" && boost::all(Value, boost::is_digit() || boost::is_any_of("-.")))
					{
						CurNumber = boost::lexical_cast<double>(Value);
					}
					int CurIntNumber = round(CurNumber);

					std::string CurString;
					for (int j = 0; j < CurIntNumber; j++)
					{
						CurString += TCHAR_TO_UTF8("★");
					}
					CurValue["value"] = CurString;
					RowData.push_back(CurValue);
					CurString = "";
					CurValue["value"] = {};
					for (int j = 0; j < 5 - CurIntNumber; j++)
					{
						CurString += TCHAR_TO_UTF8("★");
					}
					CurValue["value"] = CurString;
					RowData.push_back(CurValue);
					CurString = "";
					CurValue["value"] = {};
				}
			}
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
			Index++;
		}
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("合同评价");
	return TableData;
}
Json OServerHelper::CommodityTradingOverview(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string GoodsID = FIELDIFNULL(InData["GoodsID"]);
	std::string ReportOrderServiceListCommand = "SELECT GoodsInfo.GoodsIntentNum,SUM(GoodsContractInfo.ContractAmount),COUNT(GoodsContractInfo.ContractId) FROM GoodsContractInfo, GoodsInfo\
												WHERE GoodsContractInfo.GoodsID = GoodsInfo.GoodsID\
												AND GoodsInfo.GoodsID ='" + GoodsID + " ';";
	FMysqlResult ReportOrderServiceList = MaraidbConnector.Query(ReportOrderServiceListCommand);
	std::vector<Json> RowData;
	for (auto ReportOrderServiceListRow : ReportOrderServiceList.DataTable)
	{
		RowData.push_back(Json::parse(TCHAR_TO_UTF8(str(boost::format("{\"title\":\"意向总量\",\"xLabel\":\"位\",\"yLabel\":\"%1%\"}")) % FillPlaceholderIfNull(ReportOrderServiceListRow[0],2))));
		RowData.push_back(Json::parse(TCHAR_TO_UTF8(str(boost::format("{\"title\":\"合同总金额\",\"xLabel\":\"万\",\"yLabel\":\"%1%\"}")) % ReportOrderServiceListRow[1])));
		RowData.push_back(Json::parse(TCHAR_TO_UTF8(str(boost::format("{\"title\":\"合同数量\",\"xLabel\":\"份\",\"yLabel\":\"%1%\"}")) % ReportOrderServiceListRow[2])));
	}
	Json TableData;
	TableData["table"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("商品交易概况");
	std::vector<Json> FillaDate;
	FillaDate.push_back(TableData);
	Json TableDatas;
	TableDatas["data"] = FillaDate;
	return TableDatas;
}
Json OServerHelper::EquipmentPurchaseContract(Json InData) {
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string DemandID = FIELDIFNULL(InData["DemandID"]);
	//std::string ReportOrderServiceListCommand = "SELECT ContractCode,FlowStatus,Fontract3Type,DemandName,ContractAmount,CashDeposit,CreateDate,SignDate,FinishDate,PaymentPlan,ContractElectronicPath,ContractSignaturePath,AName,APerson,APhone,BName,BPerson,BPhone FROM (SELECT EnteEvalvate.PrvdName as AName,EnteEvalvate.ContactPerson as APerson,EnteEvalvate.ContactPhone as APhone FROM EnteEvalvate,DemandContractInfo WHERE EnteEvalvate.EnteID = DemandContractInfo.EnteId And DemandContractInfo.DemandId = '"+DemandID+"' GROUP BY AName) as A, (SELECT EnteEvalvate.PrvdName as BName,EnteEvalvate.ContactPerson as BPerson,EnteEvalvate.ContactPhone as BPhone FROM EnteEvalvate, DemandContractInfo WHERE EnteEvalvate.EnteID = DemandContractInfo.PrvdId And DemandContractInfo.DemandId = '" + DemandID + "' GROUP BY BName ) as B,DemandContractInfo WHERE DemandContractInfo.DemandId = '" + DemandID + "' GROUP BY ContractCode";
	std::string DemandContactListCommand = "SELECT ContractCode,FlowStatus,FontractType,DemandName,ContractAmount,CashDeposit,CreateDate,SignDate,FinishDate,PaymentPlan,ContractElectronicPath,ContractSignaturePath FROM DemandContractInfo WHERE DemandId = '" + DemandID + "'";

	FMysqlResult DemandContactList = MaraidbConnector.Query(DemandContactListCommand);
	std::vector<Json> RowData;
	for (auto DemandContactListRow : DemandContactList.DataTable)
	{

		for (auto Value : DemandContactListRow)
		{
			Json CurValue;
			CurValue["value"] = Value;
			RowData.push_back(CurValue);
		}
	}
	std::string EnteListCommand = "SELECT EnteID,PrvdId FROM DemandContractInfo WHERE DemandId = '" + DemandID + "'";
	FMysqlResult EnteList = MaraidbConnector.Query(EnteListCommand);
	for (auto EnteListRow : EnteList.DataTable)
	{
		Json CurValue;
		Json DemandValue;
		Json PrivValue;
		std::vector<std::string> EnteArray;
		int Index = 0;
		for (auto Value : EnteListRow)
		{
			if (Index == 0)
			{
				std::string DemandEnteCommand = "SELECT EnteName, ContactPerson, ContactPhone FROM BaseEnteInfo WHERE EnteID ='" + Value + "'";
				FMysqlResult DemandEnte = MaraidbConnector.Query(DemandEnteCommand);
				for (auto DemandEnteRow : DemandEnte.DataTable)
				{
					DemandValue["Name"] = DemandEnteRow[0];
					DemandValue["ContactPerson"] = DemandEnteRow[1];
					DemandValue["ContactPhone"] = DemandEnteRow[2];
				}
			}
			else
			{
				std::string PrivEnteCommand = "SELECT EnteName, ContactPerson, ContactPhone FROM BaseEnteInfo WHERE EnteID ='" + Value + "'";
				FMysqlResult PrivEnte = MaraidbConnector.Query(PrivEnteCommand);
				for (auto PrivEnteRow : PrivEnte.DataTable)
				{
					PrivValue["Name"] = PrivEnteRow[0];
					PrivValue["ContactPerson"] = PrivEnteRow[1];
					PrivValue["ContactPhone"] = PrivEnteRow[2];
				}
			}
			Index++;
		}
		CurValue["value"] = DemandValue["Name"];
		RowData.push_back(CurValue);
		CurValue["value"] = PrivValue["Name"];
		RowData.push_back(CurValue);
		CurValue["value"] = DemandValue["ContactPerson"];
		RowData.push_back(CurValue);
		CurValue["value"] = PrivValue["ContactPerson"];
		RowData.push_back(CurValue);
		CurValue["value"] = DemandValue["ContactPhone"];
		RowData.push_back(CurValue);
		CurValue["value"] = PrivValue["ContactPhone"];
		RowData.push_back(CurValue);
	}
	Json TableData;
	TableData["data"] = RowData;
	TableData["title"] = TCHAR_TO_UTF8("设备购买合同");
	return TableData;
}

Json OServerHelper::VariousCommodityTransactionTrends(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string GoodTypeCommand = "SELECT GoodsInfo.GoodsType From GoodsInfo,GoodsContractInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND GoodsContractInfo.SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsType;";
	FMysqlResult GoodType = MaraidbConnector.Query(GoodTypeCommand);


	std::string StartColor[4] = { "rgba(65,141,255,1)","rgba(9,227,211,1)","rgba(209,246,255,1)","rgba(65,141,255,1)" };
	std::string HighColor[4] = { "rgba(65,141,255,0.3)","rgba(9,227,211,0.3)","rgba(209,246,255,0.3)","rgba(65,141,255,0.3)" };
	std::string GradientColor[4] = { "rgba(65,141,255,0)","rgba(9,227,211,0)","rgba(209,246,255,0)","rgba(65,141,255,0)" };

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);

	std::vector<Json>XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	std::vector<Json> TableDatas;
	for (auto GoodTypeRow : GoodType.DataTable)
	{
		FMysqlResult TransactionTrendsResult = MaraidbConnector.Query(str(boost::format("SELECT MONTH(SignDate),DAY(SignDate),COUNT(ContractId) FROM GoodsContractInfo,GoodsInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND GoodsType='%1%' AND SignDate BETWEEN '%2%' AND '%3%' GROUP BY MONTH(SignDate),DAY(SignDate) ORDER BY SignDate;") % GoodTypeRow[0] % StartTime % EndTime));

		std::vector<Json>CategoryDatas;
		for (auto TransactionTrendsResultRow : TransactionTrendsResult.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = TransactionTrendsResultRow[0] + "." + TransactionTrendsResultRow[1];
			CategoryData["yLabel"] = TransactionTrendsResultRow[2];
			CategoryDatas.push_back(CategoryData);
		}

		Json TableData;
		TableData["categoryName"] = GoodTypeRow[0];
		TableData["start_color"] = StartColor[TableDatas.size() % 4];
		TableData["end_color"] = StartColor[TableDatas.size() % 4];
		TableData["high_color"] = HighColor[TableDatas.size() % 4];
		TableData["gradient_color"] = GradientColor[TableDatas.size() % 4];
		TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
		TableDatas.push_back(TableData);

	}
	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("次");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("各类商品成交趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::ES_VariousCommodityTransactionTrends(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string GoodTypeCommand = "SELECT GoodsInfo.GoodsType From GoodsInfo,GoodsContractInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND GoodsContractInfo.SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsType;";
	FMysqlResult GoodType = MaraidbConnector.Query(GoodTypeCommand);

	std::string StartColor[4] = { "rgba(255,255,255,0)", "rgba(96,199,255,0)", "rgba(0,117,255,0)" };
	std::string EndColor[4] = { "rgba(255,255,255,0.3)","rgba(96,199,255,0.3)","rgba(0,117,255,0.3)" };
	std::string HighColor[4] = { "#5ceaff","#5ceaff","#5ceaff" };

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	std::vector<Json>XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	std::vector<Json> TableDatas;
	for (auto GoodTypeRow : GoodType.DataTable)
	{
		FMysqlResult TransactionTrendsResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(SignDate),MONTH(SignDate),COUNT(ContractId) FROM GoodsContractInfo,GoodsInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND GoodsType='%1%' AND SignDate BETWEEN '%2%' AND '%3%' GROUP BY YEAR(SignDate),MONTH(SignDate) ORDER BY SignDate;") % GoodTypeRow[0] % StartTime % EndTime));

		std::vector<Json>CategoryDatas;
		for (auto TransactionTrendsResultRow : TransactionTrendsResult.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = TransactionTrendsResultRow[0] + "." + TransactionTrendsResultRow[1];
			CategoryData["yLabel"] = TransactionTrendsResultRow[2];
			CategoryDatas.push_back(CategoryData);
		}

		Json TableData;
		TableData["categoryName"] = GoodTypeRow[0];
		TableData["start_color"] = StartColor[TableDatas.size() % 4];
		TableData["end_color"] = EndColor[TableDatas.size() % 4];
		TableData["high_color"] = HighColor[TableDatas.size() % 4];
		TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
		TableDatas.push_back(TableData);

	}
	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("次");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("各类商品成交趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::ES_ContractTendency(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);


	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	std::vector<Json>XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	std::vector<Json> TableDatas;

	FMysqlResult DemandTendency = MaraidbConnector.Query(str(boost::format("SELECT YEAR(SignDate),MONTH(SignDate),COUNT(ContractId) FROM DemandContractInfo WHERE SignDate BETWEEN '%1%' AND '%2%' GROUP BY YEAR(SignDate),MONTH(SignDate)  ORDER BY SignDate;") % StartTime % EndTime));
	FMysqlResult GoodsTendency = MaraidbConnector.Query(str(boost::format("SELECT YEAR(SignDate),MONTH(SignDate),COUNT(ContractId) FROM GoodsContractInfo WHERE SignDate BETWEEN '%1%' AND '%2%' GROUP BY YEAR(SignDate),MONTH(SignDate)  ORDER BY SignDate;") % StartTime % EndTime));

	std::vector<Json> DemandCategoryDatas;

	for (auto DemandTendencyRow : DemandTendency.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = DemandTendencyRow[0] + "." + DemandTendencyRow[1];
		CategoryData["yLabel"] = DemandTendencyRow[2];
		DemandCategoryDatas.push_back(CategoryData);
	}

	std::vector<Json> GoodsCategoryDatas;

	for (auto GoodsTendencyRow : GoodsTendency.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = GoodsTendencyRow[0] + "." + GoodsTendencyRow[1];
		CategoryData["yLabel"] = GoodsTendencyRow[2];
		GoodsCategoryDatas.push_back(CategoryData);
	}
	std::vector<Json> CategoryDatas = CombineCategoryData(XLabelRangs, GoodsCategoryDatas, DemandCategoryDatas, 0);

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color_1"] = "#F2F2F2";
	TableData["high_color_1"] = "#fce700";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);


	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("单");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("合同数量变化趋势");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::AccumulatedDemandDrainPercentage(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string DemandTypeCommand = "SELECT DemandType From DemandInfo GROUP BY DemandType Limit 9;";
	FMysqlResult DemandType = MaraidbConnector.Query(DemandTypeCommand);

	std::vector<Json> CategoryDatas;
	for (auto DemandTypeRow : DemandType.DataTable)
	{
		FMysqlResult CloseDemandCount = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DemandID) FROM DemandInfo WHERE DemandType='%1%' AND FlowStatus='已关闭';")) % DemandTypeRow[0]));
		FMysqlResult ContractDemandCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DemandContractInfo.DemandID) FROM DemandInfo,DemandContractInfo WHERE DemandType='%1%' AND DemandContractInfo.DemandId=DemandInfo.DemandID;") % DemandTypeRow[0]));

		Json CategoryData;
		CategoryData["xLabel"] = DemandTypeRow[0];
		CategoryData["yLabel"] = std::to_string(lexical_cast<int>(CloseDemandCount.DataTable[0][0]) - lexical_cast<int>(ContractDemandCount.DataTable[0][0]));
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(255,255,255,0.31)";
	TableData["end_color"] = "rgba(255,255,255,0.74)";
	TableData["high_color1"] = "#F2F2F2";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("累计需求流失分类占比");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::ES_AccumulatedDemandDrainPercentage(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string DemandTypeCommand = "SELECT DemandType From DemandInfo GROUP BY DemandType Limit 9;";
	FMysqlResult DemandType = MaraidbConnector.Query(DemandTypeCommand);

	std::vector<Json> CategoryDatas;
	for (auto DemandTypeRow : DemandType.DataTable)
	{
		FMysqlResult CloseDemandCount = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DemandID) FROM DemandInfo WHERE DemandType='%1%' AND FlowStatus='已关闭';")) % DemandTypeRow[0]));
		FMysqlResult ContractDemandCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DemandContractInfo.DemandID) FROM DemandInfo,DemandContractInfo WHERE DemandType='%1%' AND DemandContractInfo.DemandId=DemandInfo.DemandID;") % DemandTypeRow[0]));

		Json CategoryData;
		CategoryData["xLabel"] = DemandTypeRow[0];
		CategoryData["yLabel"] = std::to_string(lexical_cast<int>(CloseDemandCount.DataTable[0][0]) - lexical_cast<int>(ContractDemandCount.DataTable[0][0]));
		CategoryDatas.push_back(CategoryData);
	}
	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	std::vector<Json>TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.6)";
	TableData["end_color"] = "rgba(96,199,255,0.6)";
	TableData["high_color1"] = "#F2F2F2";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("个");
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("累计需求流失分类占比");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::PixelOnTheLeftSideOfEnergyTrading(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult PendingToAuditDemand = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT DemandID) FROM DemandInfo WHERE FlowStatus='审核中';"))));
	FMysqlResult PendingToAuditGoods = MaraidbConnector.Query(TCHAR_TO_UTF8(str(boost::format("SELECT COUNT(DISTINCT GoodsID) FROM GoodsInfo WHERE GoodsStatus='审批中';"))));

	FMysqlResult DemandDealInfo = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT DemandID),SUM(ContractAmount) FROM DemandContractInfo WHERE SignDate BETWEEN '%1%' AND '%2%';") % StartTime % EndTime));
	FMysqlResult GoodsDealInfo = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT GoodsID),SUM(ContractAmount) FROM GoodsContractInfo WHERE SignDate BETWEEN '%1%' AND '%2%';") % StartTime % EndTime));

	std::vector<Json> Tables;
	Tables.push_back({
					  {"title",TCHAR_TO_UTF8("待审核需求数量")},
					  {"xLabel",""},
					  {"yLabel",PendingToAuditDemand.DataTable[0][0]}
		});
	Tables.push_back({
					  {"title",TCHAR_TO_UTF8("待审核商品数量")},
					  {"xLabel",""},
					  {"yLabel",PendingToAuditGoods.DataTable[0][0]}
		});
	Tables.push_back({
					  {"title",TCHAR_TO_UTF8("近一月成交合同数")},
					  {"xLabel",""},
					  {"yLabel",std::to_string(lexical_cast<int>(DemandDealInfo.DataTable[0][0]) + lexical_cast<int>(GoodsDealInfo.DataTable[0][0]))}
		});

	double Money = lexical_cast<double>(SaveDecimalPlaces(DemandDealInfo.DataTable[0][1])) + lexical_cast<double>(SaveDecimalPlaces(GoodsDealInfo.DataTable[0][1]));

	Tables.push_back({
					  {"title",TCHAR_TO_UTF8("近一月成交金额")},
					  {"xLabel",TCHAR_TO_UTF8("万元")},
					  {"yLabel",ConvertToTenThousand(std::to_string(Money))}
		});

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("能源交易概览");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}
Json OServerHelper::CommodityTransactionConversionRate(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string GoodsTypeCommand = "SELECT GoodsType FROM GoodsInfo WHERE GoodsType!=''  GROUP BY GoodsType ORDER BY COUNT(GoodsID) LIMIT 4;";
	FMysqlResult GoodsType = MaraidbConnector.Query(GoodsTypeCommand);

	std::vector<Json> TableDatas;
	for (auto GoodsTypeInfoRow : GoodsType.DataTable)
	{
		std::vector<Json> Datas;

		FMysqlResult GoodsViewCount = MaraidbConnector.Query(str(boost::format("SELECT SUM(GoodsViewNum),SUM(GoodsIntentNum) FROM GoodsInfo WHERE GoodsType='%1%';") % GoodsTypeInfoRow[0]));
		FMysqlResult GoodsSaleCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(GoodsContractInfo.GoodsID) FROM GoodsInfo,GoodsContractInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND GoodsType='%1%';") % GoodsTypeInfoRow[0]));

		if (GoodsViewCount.ResultIsNotNull() && GoodsSaleCount.ResultIsNotNull())
		{
			try
			{
				double IntentPersent = 0;
				double DealPersent = 0;

				if (boost::lexical_cast<double>(GoodsViewCount.DataTable[0][0]) != 0)
				{
					IntentPersent = (boost::lexical_cast<double>(GoodsViewCount.DataTable[0][1]) + boost::lexical_cast<double>(GoodsSaleCount.DataTable[0][0])) / boost::lexical_cast<double>(GoodsViewCount.DataTable[0][0]);
					DealPersent = boost::lexical_cast<double>(GoodsSaleCount.DataTable[0][0]) / boost::lexical_cast<double>(GoodsViewCount.DataTable[0][0]);
				}
				Datas.push_back({ { "value",GoodsTypeInfoRow[0]} });
				Datas.push_back({ { "value",TCHAR_TO_UTF8("访客")} });
				Datas.push_back({ { "value","100" } });
				Datas.push_back({ { "value",TCHAR_TO_UTF8("意向")} });
				Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(IntentPersent * 100))} });
				Datas.push_back({ { "value",TCHAR_TO_UTF8("成交")} });
				Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(DealPersent * 100)) } });
				TableDatas.push_back(Datas);
			}
			catch (const std::exception &)
			{

			}

		}
	}
	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("商品成交转换率");
	ReturnData["data"] = TableDatas;

	return ReturnData;
}
Json OServerHelper::DemandTransactionConversionRate(Json InData) {
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string DemandTypeInfoCommand = "SELECT DemandType FROM DemandInfo WHERE DemandType !=''  GROUP BY DemandType ORDER BY COUNT(DemandID)  LIMIT 4;";
	FMysqlResult DemandTypeInfo = MaraidbConnector.Query(DemandTypeInfoCommand);

	std::vector<Json> TableDatas;
	for (auto DemandTypeInfoRow : DemandTypeInfo.DataTable)
	{
		std::vector<Json> Datas;

		FMysqlResult DemandViewCount = MaraidbConnector.Query(str(boost::format("SELECT SUM(ViewNum),SUM(RespNum) FROM DemandInfo WHERE DemandType='%1%';") % DemandTypeInfoRow[0]));
		FMysqlResult DemandSaleCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DemandContractInfo.DemandID) FROM DemandContractInfo,DemandInfo WHERE DemandInfo.DemandID=DemandContractInfo.DemandID AND DemandType='%1%';") % DemandTypeInfoRow[0]));

		if (DemandViewCount.ResultIsNotNull() && DemandSaleCount.ResultIsNotNull())
		{
			try
			{
				double IntentPersent = 0;
				double DealPersent = 0;

				if (boost::lexical_cast<double>(DemandViewCount.DataTable[0][0]) != 0)
				{
					IntentPersent = (boost::lexical_cast<double>(DemandViewCount.DataTable[0][1]) + boost::lexical_cast<double>(DemandSaleCount.DataTable[0][0])) / boost::lexical_cast<double>(DemandViewCount.DataTable[0][0]);
					DealPersent = boost::lexical_cast<double>(DemandSaleCount.DataTable[0][0]) / boost::lexical_cast<double>(DemandViewCount.DataTable[0][0]);
				}
				Datas.push_back({ { "value",DemandTypeInfoRow[0] } });
				Datas.push_back({ { "value",TCHAR_TO_UTF8("访客") } });
				Datas.push_back({ { "value","100" } });
				Datas.push_back({ { "value",TCHAR_TO_UTF8("意向") } });
				Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(IntentPersent * 100))} });
				Datas.push_back({ { "value",TCHAR_TO_UTF8("成交") } });
				Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(DealPersent * 100)) } });
				TableDatas.push_back(Datas);
			}
			catch (const std::exception &)
			{

			}

		}
	}
	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("需求成交转换率");
	ReturnData["data"] = TableDatas;

	return ReturnData;
}

Json OServerHelper::ES_TransactionConversionRate(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::vector<Json> Datas;
	{
		std::string GoodsTypeCommand = "SELECT GoodsType FROM GoodsInfo WHERE GoodsType!=''  GROUP BY GoodsType ORDER BY COUNT(GoodsID) LIMIT 4;";
		FMysqlResult GoodsType = MaraidbConnector.Query(GoodsTypeCommand);

		std::vector<Json> TableDatas;
		for (auto GoodsTypeInfoRow : GoodsType.DataTable)
		{
			std::vector<Json> Datas;

			FMysqlResult GoodsViewCount = MaraidbConnector.Query(str(boost::format("SELECT SUM(GoodsViewNum),SUM(GoodsIntentNum) FROM GoodsInfo WHERE GoodsType='%1%';") % GoodsTypeInfoRow[0]));
			FMysqlResult GoodsSaleCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(GoodsContractInfo.GoodsID) FROM GoodsInfo,GoodsContractInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND GoodsType='%1%';") % GoodsTypeInfoRow[0]));

			if (GoodsViewCount.ResultIsNotNull() && GoodsSaleCount.ResultIsNotNull())
			{
				try
				{
					double IntentPersent = 0;
					double DealPersent = 0;

					if (boost::lexical_cast<double>(GoodsViewCount.DataTable[0][0]) != 0)
					{
						IntentPersent = (boost::lexical_cast<double>(GoodsViewCount.DataTable[0][1]) + boost::lexical_cast<double>(GoodsSaleCount.DataTable[0][0])) / boost::lexical_cast<double>(GoodsViewCount.DataTable[0][0]);
						DealPersent = boost::lexical_cast<double>(GoodsSaleCount.DataTable[0][0]) / boost::lexical_cast<double>(GoodsViewCount.DataTable[0][0]);
					}
					Datas.push_back({ { "value",GoodsTypeInfoRow[0]} });
					Datas.push_back({ { "value",TCHAR_TO_UTF8("访客") } });
					Datas.push_back({ { "value","100" } });
					Datas.push_back({ { "value",TCHAR_TO_UTF8("意向") } });
					Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(IntentPersent * 100))} });
					Datas.push_back({ { "value",TCHAR_TO_UTF8("成交") } });
					Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(DealPersent * 100)) } });
					TableDatas.push_back(Datas);
				}
				catch (const std::exception &)
				{

				}

			}
		}
		Json Data;
		Data["title"] = TCHAR_TO_UTF8("商品成交转换率");
		Data["data"] = TableDatas;
		Datas.push_back(Data);
	}

	{
		std::string DemandTypeInfoCommand = "SELECT DemandType FROM DemandInfo WHERE DemandType !=''  GROUP BY DemandType ORDER BY COUNT(DemandID) LIMIT 4;";
		FMysqlResult DemandTypeInfo = MaraidbConnector.Query(DemandTypeInfoCommand);

		std::vector<Json> TableDatas;
		for (auto DemandTypeInfoRow : DemandTypeInfo.DataTable)
		{
			std::vector<Json> Datas;

			FMysqlResult DemandViewCount = MaraidbConnector.Query(str(boost::format("SELECT SUM(ViewNum),SUM(RespNum) FROM DemandInfo WHERE DemandType='%1%';") % DemandTypeInfoRow[0]));
			FMysqlResult DemandSaleCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DemandContractInfo.DemandID) FROM DemandContractInfo,DemandInfo WHERE DemandInfo.DemandID=DemandContractInfo.DemandID AND DemandType='%1%';") % DemandTypeInfoRow[0]));

			if (DemandViewCount.ResultIsNotNull() && DemandSaleCount.ResultIsNotNull())
			{
				try
				{
					double IntentPersent = 0;
					double DealPersent = 0;

					if (boost::lexical_cast<double>(DemandViewCount.DataTable[0][0]) != 0)
					{
						IntentPersent = (boost::lexical_cast<double>(DemandViewCount.DataTable[0][1]) + boost::lexical_cast<double>(DemandSaleCount.DataTable[0][0])) / boost::lexical_cast<double>(DemandViewCount.DataTable[0][0]);
						DealPersent = boost::lexical_cast<double>(DemandSaleCount.DataTable[0][0]) / boost::lexical_cast<double>(DemandViewCount.DataTable[0][0]);
					}

					Datas.push_back({ { "value",DemandTypeInfoRow[0] } });
					Datas.push_back({ { "value",TCHAR_TO_UTF8("访客") } });
					Datas.push_back({ { "value","100" } });
					Datas.push_back({ { "value",TCHAR_TO_UTF8("意向") } });
					Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(IntentPersent * 100))} });
					Datas.push_back({ { "value",TCHAR_TO_UTF8("成交") } });
					Datas.push_back({ { "value",SaveDecimalPlaces(std::to_string(DealPersent * 100)) } });
					TableDatas.push_back(Datas);

				}
				catch (const std::exception &)
				{

				}

			}
		}
		Json Data;
		Data["title"] = TCHAR_TO_UTF8("需求成交转换率");
		Data["data"] = TableDatas;
		Datas.push_back(Data);
	}
	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::ES_ProportionTransactionCategories(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::vector<Json> Datas;

	{
		std::string GoodsSaleInfoCommand = "SELECT GoodsType,COUNT(GoodsContractInfo.GoodsID) FROM GoodsContractInfo,GoodsInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsType ORDER BY COUNT(GoodsContractInfo.GoodsID);";
		FMysqlResult GoodsSaleInfo = MaraidbConnector.Query(GoodsSaleInfoCommand);

		std::vector<Json> CategoryDatas;
		for (auto GoodsSaleInfoRow : GoodsSaleInfo.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = GoodsSaleInfoRow[0];
			CategoryData["yLabel"] = GoodsSaleInfoRow[1];
			CategoryDatas.push_back(CategoryData);
		}
		std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

		std::vector<Json>TableDatas;
		Json TableData;
		TableData["categoryName"] = "";
		TableData["start_color"] = "#00FDAD";
		TableData["end_color"] = "#00FDAD";
		TableData["high_color"] = "#00FDAD";
		TableData["gradient_color"] = "#00FDAD";
		TableData["categoryData"] = CategoryDatas;
		TableDatas.push_back(TableData);

		std::vector<Json> Tables;
		Json Table;
		Table["title"] = "";
		Table["unit"] = TCHAR_TO_UTF8("个");
		Table["data"] = TableDatas;
		Tables.push_back(Table);

		Json Data;

		Data["title"] = TCHAR_TO_UTF8("商品成交分类占比");
		Data["table"] = Tables;

		Datas.push_back(Data);
	}

	{
		std::string DemmandCountCommand = "SELECT DemandType,COUNT(DemandContractInfo.DemandID) FROM DemandContractInfo,DemandInfo WHERE DemandContractInfo.DemandId=DemandInfo.DemandID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY DemandType ORDER BY COUNT(DemandContractInfo.DemandID);";
		FMysqlResult DemandCount = MaraidbConnector.Query(DemmandCountCommand);

		std::vector<Json> CategoryDatas;
		for (auto DemandCountRow : DemandCount.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = DemandCountRow[0];
			CategoryData["yLabel"] = DemandCountRow[1];
			CategoryDatas.push_back(CategoryData);
		}

		std::vector<Json> TableDatas;
		Json TableData;
		TableData["categoryName"] = "";
		TableData["start_color"] = "#00FDAD";
		TableData["end_color"] = "#00FDAD";
		TableData["high_color"] = "#00FDAD";
		TableData["gradient_color"] = "#00FDAD";
		TableData["categoryData"] = CategoryDatas;
		TableDatas.push_back(TableData);

		std::vector<Json> Tables;
		Json Table;
		Table["title"] = "";
		Table["unit"] = "";
		Table["data"] = TableDatas;
		Tables.push_back(Table);

		Json Data;
		Data["title"] = TCHAR_TO_UTF8("需求成交分类占比");
		Data["table"] = Tables;
		Datas.push_back(Data);
	}

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnergyUseTrends_Year(Json InData)
{
	using namespace boost;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::vector<Json> TableDatas;

	FMysqlResult EnergyConsumResult = MaraidbConnector.Query(str(boost::format("Call GetEnteEnergyConsumption('%1%','%2%','%3%');") % ElecMetID % StartTime % EndTime));

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '-', EXLabelStepDateIterator::Month);

	date Now = day_clock::local_day();
	std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig);


	int CategoryIndex[4] = { 1,5,6,7 };
	std::string CategoryNames[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
	Json Data;
	for (int Index=0;Index<4;Index++)
	{
		std::vector<Json> CategoryData;
		for (auto EnergyConsumResultRow: EnergyConsumResult.DataTable)
		{
			CategoryData.push_back({ {"xLabel",ConvertDateToMonth(EnergyConsumResultRow[0],'-')},{"yLabel",EnergyConsumResultRow[CategoryIndex[Index]]} });
		}
		Data[CategoryNames[Index]] = FillCategoryData(XLabelRangsCurrentTime,CategoryData);
	}

	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}

Json OServerHelper::EnergyUseTrends_Month(Json InData)
{
	using namespace boost;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::vector<Json> TableDatas;

	FMysqlResult EnergyConsumResult = MaraidbConnector.Query(str(boost::format("Call GetEnteEnergyConsumption_Day('%1%','%2%','%3%');") % ElecMetID % StartTime % EndTime));

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '-', EXLabelStepDateIterator::Day);

	date Now = day_clock::local_day();
	std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig);

	Json Data;

	std::vector<Json> CategoryData;
	for (auto EnergyConsumResultRow : EnergyConsumResult.DataTable)
	{
		CategoryData.push_back({ {"xLabel",ConvertDateToDay(EnergyConsumResultRow[0],'-')},{"yLabel",EnergyConsumResultRow[5]} });
	}
	Data[TCHAR_TO_UTF8("电")] = FillCategoryData(XLabelRangsCurrentTime, CategoryData);


	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}

Json OServerHelper::EnergyUseTrends_Day(Json InData)
{
	using namespace boost;
	using namespace  boost::posix_time;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string SearchDate = FIELDIFNULL(InData["SearchDate"]);

	std::vector<Json> TableDatas;

	FMysqlResult EnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT DataTime,DataValue FROM LoadData WHERE ElecMetID='%1%' AND SearchDate='%2%' ORDER BY DataTime;") % ElecMetID % SearchDate));

	std::vector<Json> XLabelRangs;
	ptime XLabelTime = time_from_string(SearchDate + " 00:00:00");
	for (int Index = 0; Index < 24; Index++)
	{
		std::string CurrentTime = to_simple_string(XLabelTime.time_of_day());
		CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
		Json XLabelRang;
		XLabelRang["label"] = CurrentTime;
		XLabelRangs.push_back(XLabelRang);
		XLabelTime = XLabelTime + hours(1);
	}
	Json Data;
	std::vector<Json> CategoryData;
	for (auto EnergyConsumResultRow : EnergyConsumResult.DataTable)
	{
		CategoryData.push_back({ {"xLabel",EnergyConsumResultRow[0]},{"yLabel",SaveDecimalPlaces(EnergyConsumResultRow[1],2)} });
	}
	std::vector<Json> EmptyJsonArray;

	Data[TCHAR_TO_UTF8("日负荷-区域属性")] = EmptyJsonArray;

	Data[TCHAR_TO_UTF8("日负荷-总负荷")] = FillCategoryData(XLabelRangs, CategoryData);


	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}

Json OServerHelper::EnergyUseTrends(Json InData)
{
	using namespace boost;
	using namespace  boost::posix_time;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string SearchDate = FIELDIFNULL(InData["SearchDate"]);

	std::vector<Json> TableDatas;

	FMysqlResult EnteInfo = MaraidbConnector.Query(str(boost::format("SELECT EnteName,Address FROM BaseEnteInfo WHERE ElecMetID='%1%';") % ElecMetID));

	FMysqlResult EnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT DataTime,DataValue FROM LoadData WHERE ElecMetID='%1%' AND SearchDate='%2%' ORDER BY DataTime;") % ElecMetID % SearchDate));

	std::vector<Json> XLabelRangs;
	ptime XLabelTime = time_from_string(SearchDate + " 00:00:00");
	for (int Index = 0; Index < 96; Index++)
	{
		std::string CurrentTime = to_simple_string(XLabelTime.time_of_day());
		CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
		Json XLabelRang;
		XLabelRang["label"] = CurrentTime;
		XLabelRangs.push_back(XLabelRang);
		XLabelTime = XLabelTime + minutes(15);
	}
	Json Data;
	std::vector<Json> CategoryData;
	for (auto EnergyConsumResultRow : EnergyConsumResult.DataTable)
	{
		CategoryData.push_back({ {"xLabel",EnergyConsumResultRow[0]},{"yLabel",SaveDecimalPlaces(EnergyConsumResultRow[1],4)} });
	}

	std::vector<Json> EnteLine;
	CategoryData = FillCategoryData(XLabelRangs, CategoryData);
	for (auto CategoryDataRow: CategoryData)
	{
		EnteLine.push_back(CategoryDataRow["yLabel"]);
	}
	Data["enteName"] = EnteInfo.ResultIsNotNull() ? EnteInfo.DataTable[0][0] : "----";
	Data["enteAdress"] = EnteInfo.ResultIsNotNull() ? EnteInfo.DataTable[0][1] : "------------";
	Data["enteLine"] = EnteLine;

	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}

Json OServerHelper::AreaOverView(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionAreaResult = MaraidbConnector.Query(str(boost::format("SELECT Area FROM BaseRegionInfo WHERE BaseRegionInfo.RegionID='%1%';") % RegionID));
	FMysqlResult RegionEnteCountResult = MaraidbConnector.Query(str(boost::format("CALL GetRegionInfo_EnteNum('%1%')") % RegionID));
	FMysqlResult RegionIndustryCountResult = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT IndustryID) FROM RegionIndustry WHERE FIND_IN_SET(RegionIndustry.RegionID,GetChildrenRegion('%1%'))") % RegionID));
	FMysqlResult RegionClearEnergyRationResult = MaraidbConnector.Query(str(boost::format("SELECT AVG(ClearEnergyRation) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % StartTime % EndTime));

	Json ReturnData = {
		{"data",
			{
				{
					{"title",TCHAR_TO_UTF8("区域概况")},
					{"table",
						{
							{{"title",TCHAR_TO_UTF8("面积")},{"xLabel",TCHAR_TO_UTF8("km²")},{"yLabel",SavePrecision(SaveDecimalPlaces(RegionAreaResult.DataTable[0][0]),4)}},
							{{"title",TCHAR_TO_UTF8("接入企业")},{"xLabel",TCHAR_TO_UTF8("户")},{"yLabel",RegionEnteCountResult.DataTable[0][0]}},
							{{"title",TCHAR_TO_UTF8("覆盖行业")},{"xLabel",TCHAR_TO_UTF8("类")},{"yLabel",RegionIndustryCountResult.DataTable[0][0]}},
							{{"title",TCHAR_TO_UTF8("清洁能源占比")},{"xLabel","%"},{"yLabel",SaveDecimalPlaces(RegionClearEnergyRationResult.DataTable[0][0])}},
						}
					}
				}
			}
		}
	};

	return ReturnData;
}
Json OServerHelper::ComparisonChartOfSupplyandDemandForTheYear(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("年度供需对比"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));
	TableTitle.push_back(TCHAR_TO_UTF8("电"));
	TableTitle.push_back(TCHAR_TO_UTF8("水"));
	TableTitle.push_back(TCHAR_TO_UTF8("气"));

	std::vector<std::string> Unit;
	Unit.push_back(TCHAR_TO_UTF8("(万tce)"));
	Unit.push_back(TCHAR_TO_UTF8("(万kW·h)"));
	Unit.push_back(TCHAR_TO_UTF8("(万m³)"));
	Unit.push_back(TCHAR_TO_UTF8("(万m³)"));

	std::vector<std::string> CategoryName;
	CategoryName.push_back(TCHAR_TO_UTF8("供"));
	CategoryName.push_back(TCHAR_TO_UTF8("需"));

	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, true, true, false,
		'-', EXLabelStepDateIterator::Month));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(96,199,255,0)");
	StartColor.push_back("rgba(76,162,255,0)");

	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(96,199,255,0.4)");
	EndColor.push_back("rgba(76,162,255,0.4)");

	std::vector<std::string> HighColor;
	HighColor.push_back("rgba(96,199,255,1)");
	HighColor.push_back("rgba(76,162,255,1)");


	std::vector<std::string> GradientColor;
	GradientColor.push_back("#8FC31F5A");

	std::string RegionID = InData["RegionID"];
	std::string StartTime = InData["StartTime"];
	std::string EndTime = InData["EndTime"];

	std::string RegionEnergyEfficiencyCommand = "SELECT YEAR(CreateDate),MONTH(CreateDate),SynEnergySupply,SynEnergyConsum,ElecSupply,ElecConsumSelf,WaterSupply,WaterConsumSelf,GasSupply,GasConsumSelf FROM RegionEnergyEfficiencyMonth WHERE RegionID='" + RegionID + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' ORDER BY CreateDate ;";
	FMysqlResult RegionEnergyEfficiency = MaraidbConnector.Query(RegionEnergyEfficiencyCommand);

	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{
		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{
			date Now = day_clock::local_day();
			std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);;

			std::vector<Json>TableDatas;
			for (int CategoryIndex = 0; CategoryIndex < CategoryName.size(); CategoryIndex++)
			{
				Json TableData;
				TableData["categoryName"] = CategoryName[CategoryIndex % CategoryName.size()];
				TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
				TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
				TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];
				TableData["gradient_color"] = GradientColor[CategoryIndex % GradientColor.size()];

				std::vector<Json> CategoryDatas;
				for (auto RegionEnergyEfficiencyRow : RegionEnergyEfficiency.DataTable)
				{
					Json CategoryData;
					CategoryData["xLabel"] = RegionEnergyEfficiencyRow[0] + "-" + RegionEnergyEfficiencyRow[1];
					CategoryData["yLabel"] = ConvertToTenThousand(RegionEnergyEfficiencyRow[(float)TableIndex * 2 + (float)CategoryIndex + 2]);
					CategoryDatas.push_back(CategoryData);
				}
				TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
				TableDatas.push_back(TableData);
			}
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);
			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[TableIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
		}
		Json Data;
		Data["title"] = DataTitle[DataIndex];
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;
}
Json OServerHelper::EnergyEvaluateIndicator(Json InData)
{
	using namespace boost;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	date D_StartTime = from_string(StartTime);
	date D_EndTime = from_string(EndTime);
	FMysqlResult CurrentResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum),SUM(OutputValue),SUM(EnergyConsumUnitArea),SUM(CarbonEmission) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));

	FMysqlResult AdvanceResult = MaraidbConnector.Query(str(boost::format("SELECT AVG(OutputValueAdvanced),AVG(EnergyConsumUnitAreaAdvanced),AVG(CarbonEmissionAdvanced) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));

	date D_PreMonthStartTime = D_StartTime - months(1);
	date D_PreMonthEndTime = D_EndTime - months(1);
	FMysqlResult PreMonthResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum),SUM(OutputValue),SUM(EnergyConsumUnitArea),SUM(CarbonEmission) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % to_iso_extended_string(D_PreMonthStartTime) % to_iso_extended_string(D_PreMonthEndTime)));

	date D_PreYearStartTime = D_StartTime - years(1);
	date D_PreYearEndTime = D_EndTime - years(1);
	FMysqlResult PreYearResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum),SUM(OutputValue),SUM(EnergyConsumUnitArea),SUM(CarbonEmission) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));

	string RowHeaderName[4] = { TCHAR_TO_UTF8("总能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("单位建筑面积能耗"),TCHAR_TO_UTF8("碳排放") };
	string Unit[4] = { TCHAR_TO_UTF8("万tce"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("tce/km²"),TCHAR_TO_UTF8("万t·CO₂") };
	std::vector<Json> Tables;
	for (int i = 0; i < 4; i++)
	{
		Json Table;
		Json RowHeader = { {{{"headerName",RowHeaderName[i]}},{{"headerName",TCHAR_TO_UTF8("同比")}},{{"headerName",TCHAR_TO_UTF8("环比")}},{{"headerName",i == 0 ? "------" : TCHAR_TO_UTF8("先进水平")}}} };
		double YearGrowth = 100;
		double MonthGrowth = 100;

		if (ConvertEmptyStringToZero(PreYearResult.DataTable[0][i])!="0")
		{
			YearGrowth = (boost::lexical_cast<double>(ConvertEmptyStringToZero(CurrentResult.DataTable[0][i])) - boost::lexical_cast<double>(ConvertEmptyStringToZero(PreYearResult.DataTable[0][i]))) / boost::lexical_cast<double>(ConvertEmptyStringToZero(PreYearResult.DataTable[0][i]))*100;
		}
		if (ConvertEmptyStringToZero(PreMonthResult.DataTable[0][i])!="0")
		{
			MonthGrowth = (boost::lexical_cast<double>(ConvertEmptyStringToZero(CurrentResult.DataTable[0][i])) - boost::lexical_cast<double>(ConvertEmptyStringToZero(PreMonthResult.DataTable[0][i]))) / boost::lexical_cast<double>(ConvertEmptyStringToZero(PreMonthResult.DataTable[0][i]))*100;
		}

		Json Data = {
						{{"value",ConvertEmptyStringToZero(CurrentResult.DataTable[0][i]) == "0" ? "------" : (i == 0 ? ConvertToTenThousand(CurrentResult.DataTable[0][i]) : SaveDecimalPlaces(CurrentResult.DataTable[0][i])) + " " + Unit[i]}},
						{{"value",ConvertEmptyStringToZero(PreYearResult.DataTable[0][i]) == "0" ? "------" : (str(boost::format("%.2f") % YearGrowth) + "%")}},
						{{"value",ConvertEmptyStringToZero(PreMonthResult.DataTable[0][i]) == "0" ? "------" : (str(boost::format("%.2f") % MonthGrowth) + "%")}},
						{{"value",(i == 0 || ConvertEmptyStringToZero(AdvanceResult.DataTable[0][i - 1]) == "0" || ConvertEmptyStringToZero(AdvanceResult.DataTable[0][i - 1]) == "") ? "------" : ConvertEmptyStringToZero(AdvanceResult.DataTable[0][i - 1])}}
		};

		Table["rowHeader"] = RowHeader;
		Table["data"] = Data;

		Tables.push_back(Table);
	}
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("能级评价指标");
	Data["table"] = Tables;
	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnergyEvaluateIndicator_AIIsland(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string ElecMetID = "1352958966";
	date D_StartTime = from_string(StartTime);
	date D_EndTime = from_string(EndTime);

	FMysqlResult CurrentResult_SynEnergyConsum = MaraidbConnector.Query(str(boost::format("Call GetEnteEnergyConsumption_SUM('%1%','%2%','%3%');") % ElecMetID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));
	FMysqlResult CurrentResult_OutputValue = MaraidbConnector.Query(str(boost::format("SELECT SUM(OutputValue) FROM EnteOutputValue WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));
	FMysqlResult CurrentResult_EnergyConsumUnitArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(AreaTce) FROM EnteEnergyConsumUnitArea WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));
	FMysqlResult CurrentResult_CarbonEmission = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteCarbonEmission WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));

	FMysqlResult CurrentResults[4] = { CurrentResult_SynEnergyConsum ,CurrentResult_OutputValue ,CurrentResult_EnergyConsumUnitArea ,CurrentResult_CarbonEmission };

	FMysqlResult AdvanceResult = MaraidbConnector.Query(str(boost::format("SELECT AVG(OutputValueAdvanced),AVG(EnergyConsumUnitAreaAdvanced),AVG(CarbonEmissionAdvanced) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % to_iso_extended_string(D_StartTime) % to_iso_extended_string(D_EndTime)));

	date D_PreMonthStartTime = D_StartTime - months(1);
	date D_PreMonthEndTime = D_EndTime - months(1);

	FMysqlResult PreMonthResult_SynEnergyConsum = MaraidbConnector.Query(str(boost::format("Call GetEnteEnergyConsumption_SUM('%1%','%2%','%3%');") % ElecMetID % to_iso_extended_string(D_PreMonthStartTime) % to_iso_extended_string(D_PreMonthEndTime)));
	FMysqlResult PreMonthResult_OutputValue = MaraidbConnector.Query(str(boost::format("SELECT SUM(OutputValue) FROM EnteOutputValue WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_PreMonthStartTime) % to_iso_extended_string(D_PreMonthEndTime)));
	FMysqlResult PreMonthResult_EnergyConsumUnitArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(AreaTce) FROM EnteEnergyConsumUnitArea WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_PreMonthStartTime) % to_iso_extended_string(D_PreMonthEndTime)));
	FMysqlResult PreMonthResult_CarbonEmission = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteCarbonEmission WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_PreMonthStartTime) % to_iso_extended_string(D_PreMonthEndTime)));

	FMysqlResult PreMonthResults[4] = { PreMonthResult_SynEnergyConsum ,PreMonthResult_OutputValue ,PreMonthResult_EnergyConsumUnitArea ,PreMonthResult_CarbonEmission };


	date D_PreYearStartTime = D_StartTime - years(1);
	date D_PreYearEndTime = D_EndTime - years(1);

	FMysqlResult PreYearResult_SynEnergyConsum = MaraidbConnector.Query(str(boost::format("Call GetEnteEnergyConsumption_SUM('%1%','%2%','%3%');") % ElecMetID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));
	FMysqlResult PreYearResult_OutputValue = MaraidbConnector.Query(str(boost::format("SELECT SUM(OutputValue) FROM EnteOutputValue WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));
	FMysqlResult PreYearResult_EnergyConsumUnitArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(AreaTce) FROM EnteEnergyConsumUnitArea WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));
	FMysqlResult PreYearResult_CarbonEmission = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteCarbonEmission WHERE ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));

	FMysqlResult PreYearResults[4] = { PreYearResult_SynEnergyConsum ,PreYearResult_OutputValue ,PreYearResult_EnergyConsumUnitArea ,PreYearResult_CarbonEmission };

	string RowHeaderName[4] = { TCHAR_TO_UTF8("总能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("单位建筑面积能耗"),TCHAR_TO_UTF8("碳排放") };
	string Unit[4] = { TCHAR_TO_UTF8("万tce"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("tce/m²"),TCHAR_TO_UTF8("t·CO₂") };
	std::vector<Json> Tables;
	for (int i = 0; i < 4; i++)
	{
		Json Table;
		Json RowHeader = { {{{"headerName",RowHeaderName[i]}},{{"headerName",TCHAR_TO_UTF8("同比")}},{{"headerName",TCHAR_TO_UTF8("环比")}},{{"headerName",i == 0 ? "------" : TCHAR_TO_UTF8("先进水平")}}} };
		double YearGrowth = 0;
		double MonthGrowth = 0;
		if ((boost::lexical_cast<double>(ConvertEmptyStringToZero(PreYearResults[i].DataTable[0][0]))) != 0)
		{
			YearGrowth = boost::lexical_cast<double>(ConvertEmptyStringToZero(CurrentResults[i].DataTable[0][0])) / boost::lexical_cast<double>(ConvertEmptyStringToZero(PreYearResults[i].DataTable[0][0])) - 1;
		}
		if ((boost::lexical_cast<double>(ConvertEmptyStringToZero(PreMonthResults[i].DataTable[0][0]))) != 0)
		{
			MonthGrowth = boost::lexical_cast<double>(ConvertEmptyStringToZero(CurrentResults[i].DataTable[0][0])) / boost::lexical_cast<double>(ConvertEmptyStringToZero(PreMonthResults[i].DataTable[0][0])) - 1;
		}

		Json Data = {
						{{"value",ConvertEmptyStringToZero(CurrentResults[i].DataTable[0][0]) == "0" ? "------" : (i == 0 ? ConvertToTenThousand(CurrentResults[i].DataTable[0][0]) : SaveDecimalPlaces(CurrentResults[i].DataTable[0][0])) + " " + Unit[i]}},
						{{"value",YearGrowth == 0 ? "------" : (str(boost::format("%.2f") % YearGrowth) + "%")}},
						{{"value",MonthGrowth == 0 ? "------" : (str(boost::format("%.2f") % MonthGrowth) + "%")}},
						{{"value",(i == 0 || AdvanceResult.DataTable[0][i - 1] == "0" || AdvanceResult.DataTable[0][i - 1] == "") ? "------" : AdvanceResult.DataTable[0][i - 1]}}
		};

		Table["rowHeader"] = RowHeader;
		Table["data"] = Data;

		Tables.push_back(Table);
	}
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("能级评价指标");
	Data["table"] = Tables;
	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::SynEnergyConsumLinkRatio(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult SynEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),SynEnergyConsum FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % RegionID % StartTime % EndTime));

	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	date Now = day_clock::local_day();
	std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig);

	std::vector<Json> CategoryDatas;
	for (auto SynEnergyConsumResultRow : SynEnergyConsumResult.DataTable)
	{
		Json CategoryData;
		CategoryData["xLabel"] = SynEnergyConsumResultRow[0] + "." + SynEnergyConsumResultRow[1];
		CategoryData["yLabel"] = ConvertToTenThousand(SynEnergyConsumResultRow[2]);
		CategoryDatas.push_back(CategoryData);
	}
	std::vector<Json> TableDatas;
	Json TableData;

	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color1"] = "#ed5f00";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
	TableDatas.push_back(TableData);

	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = TCHAR_TO_UTF8("万tce");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("年度综合能耗环比");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::SynEnergyConsumLinkRatio_AIIsland(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;
	using namespace boost;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	/*std::string RegionID = InData["RegionID"];*/
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ElecMetID = "1352958966";
	//FMysqlResult SynEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),SynEnergyConsum FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % RegionID % StartTime % EndTime));
	std::string EnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption('" + ElecMetID + "','" + StartTime + "','" + EndTime + "')";
	FMysqlResult SynEnergyConsumResult = MaraidbConnector.Query(EnteEnergyConsumptionCommand);
	FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	date Now = day_clock::local_day();
	std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig);

	std::vector<Json> CategoryDatas;
	for (auto SynEnergyConsumResultRow : SynEnergyConsumResult.DataTable)
	{
		Json CategoryData;
		/*std::list<std::string> SplitArray;
		boost::split(SplitArray, SynEnergyConsumResultRow[0], is_any_of("-"));
		if (SplitArray.size() < 2)continue;*/
		/*CategoryData["xLabel"] = SplitArray.front() + "." + SplitArray.back(), XLabelRangCofig;*/
		CategoryData["xLabel"] = ConvertDateToMonth(SynEnergyConsumResultRow[0]);
		CategoryData["yLabel"] = SynEnergyConsumResultRow[1];
		CategoryDatas.push_back(CategoryData);
	}
	std::vector<Json> TableDatas;
	Json TableData;

	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color1"] = "#ed5f00";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
	TableDatas.push_back(TableData);

	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = TCHAR_TO_UTF8("万tce");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("年度综合能耗环比-人工智能岛");
	Data["table"] = Tables;

	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::EnergyEfficiencyLinkRatio_AIIsland(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("万元产值能耗"));
	DataTitle.push_back(TCHAR_TO_UTF8("单位面积能耗"));
	DataTitle.push_back(TCHAR_TO_UTF8("度电经济增加值"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));

	std::vector<std::string> CategoryName;
	CategoryName.push_back("");

	std::vector<std::string> Unit;
	Unit.push_back(TCHAR_TO_UTF8("tce/万元"));
	Unit.push_back(TCHAR_TO_UTF8("tce/k㎡"));
	Unit.push_back(TCHAR_TO_UTF8("元/kW·h"));

	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, true, true, false,
		'.', EXLabelStepDateIterator::Month));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(96,199,255,0.3)");


	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(96,199,255,1)");


	std::vector<std::string> HighColor;
	HighColor.push_back("#5ceaff");

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	//std::string RegionID = InData["RegionID"];
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string ElecMetID = "1352958966";
	std::string EnteEnergyConsumptionCommand = "CALL GetEnteMonthConsumeUnitArea('" + ElecMetID + "','" + StartTime + "','" + EndTime + "')";
	FMysqlResult EnergyEfficiencyConsumResult = MaraidbConnector.Query(EnteEnergyConsumptionCommand);
	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{

		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{

			date Now = day_clock::local_day();
			std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);
			std::vector<Json>TableDatas;

			for (int CategoryIndex = 0; CategoryIndex < CategoryName.size(); CategoryIndex++)
			{
				if (DataTitle[DataIndex] != TCHAR_TO_UTF8("单位面积能耗"))
				{
					continue;
				}
				Json TableData;
				TableData["categoryName"] = CategoryName[CategoryIndex];
				TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
				TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
				TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];
				std::vector<Json> CategoryDatas;
				for (auto EnergyEfficiencyConsumResultRow : EnergyEfficiencyConsumResult.DataTable)
				{

					Json CategoryData;
					CategoryData["xLabel"] = EnergyEfficiencyConsumResultRow[0] + "." + EnergyEfficiencyConsumResultRow[1];
					CategoryData["yLabel"] = SaveDecimalPlaces(EnergyEfficiencyConsumResultRow[2],2) ;
					CategoryDatas.push_back(CategoryData);
				}

				TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
				TableDatas.push_back(TableData);
				CategoryIndex++;

			}
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);
			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[DataIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
		}
		Json Data;
		Data["title"] = DataTitle[DataIndex];
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;


}

Json OServerHelper::EnergyEfficiencyLinkRatio(Json InData)
{
	using namespace boost;
	using namespace boost::gregorian;
	using namespace std;

	std::vector<std::string> DataTitle;
	DataTitle.push_back(TCHAR_TO_UTF8("万元产值能耗"));
	DataTitle.push_back(TCHAR_TO_UTF8("单位面积能耗"));
	DataTitle.push_back(TCHAR_TO_UTF8("度电经济增加值"));

	std::vector<std::string> TableTitle;
	TableTitle.push_back(TCHAR_TO_UTF8("总"));

	std::vector<std::string> CategoryName;
	CategoryName.push_back("");

	std::vector<std::string> Unit;
	Unit.push_back(TCHAR_TO_UTF8("tce/万元"));
	Unit.push_back(TCHAR_TO_UTF8("tce/km²"));
	Unit.push_back(TCHAR_TO_UTF8("元/kW·h"));

	std::vector<FXLabelRangCofig> XLabelRangCofig;
	XLabelRangCofig.push_back(FXLabelRangCofig(1, true, true, false,
		'.', EXLabelStepDateIterator::Month));

	std::vector<std::string> StartColor;
	StartColor.push_back("rgba(96,199,255,0.3)");


	std::vector<std::string> EndColor;
	EndColor.push_back("rgba(96,199,255,1)");


	std::vector<std::string> HighColor;
	HighColor.push_back("#5ceaff");

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EnergyEfficiencyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT YEAR(CreateDate),MONTH(CreateDate),OutputValue,EnergyConsumUnitArea,EnergyEcnoValueAdd FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % RegionID % StartTime % EndTime));

	std::vector<Json> Datas;
	for (int DataIndex = 0; DataIndex < DataTitle.size(); DataIndex++)
	{
		std::vector<Json> Tables;
		for (int TableIndex = 0; TableIndex < TableTitle.size(); TableIndex++)
		{
			date Now = day_clock::local_day();
			std::vector<Json> XLabelRangsCurrentTime = GetXLabelRang(StartTime, EndTime > to_iso_extended_string(Now) ? to_iso_extended_string(Now) : EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			std::vector<Json>TableDatas;

			for (int CategoryIndex = 0; CategoryIndex < CategoryName.size(); CategoryIndex++)
			{
				Json TableData;
				TableData["categoryName"] = CategoryName[CategoryIndex];
				TableData["start_color"] = StartColor[CategoryIndex % StartColor.size()];
				TableData["end_color"] = EndColor[CategoryIndex % EndColor.size()];
				TableData["high_color"] = HighColor[CategoryIndex % HighColor.size()];

				std::vector<Json> CategoryDatas;
				for (auto EnergyEfficiencyConsumResultRow : EnergyEfficiencyConsumResult.DataTable)
				{
					Json CategoryData;
					CategoryData["xLabel"] = EnergyEfficiencyConsumResultRow[0] + "." + EnergyEfficiencyConsumResultRow[1];
					CategoryData["yLabel"] = EnergyEfficiencyConsumResultRow[(float)DataIndex + 2];
					CategoryDatas.push_back(CategoryData);
				}
				TableData["categoryData"] = FillCategoryData(XLabelRangsCurrentTime, CategoryDatas);
				TableDatas.push_back(TableData);
				CategoryIndex++;
			}
			std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig[TableIndex % XLabelRangCofig.size()]);

			Json Table;
			Table["title"] = TableTitle[TableIndex];
			Table["unit"] = Unit[DataIndex];
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
		}
		Json Data;
		Data["title"] = DataTitle[DataIndex];
		Data["table"] = Tables;
		Datas.push_back(Data);
	}
	Json TableData;
	TableData["data"] = Datas;

	return TableData;


}

Json OServerHelper::AreaEnergyConsumStructure(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EnergyConsumStructureResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(ElecConsum),SUM(WaterConsum),SUM(GasConsum) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % RegionID % StartTime % EndTime));

	std::vector<Json> CategoryDatas;
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("电")},{"yLabel",ConvertToTenThousand(EnergyConsumStructureResult.DataTable[0][0])} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("水")},{"yLabel",ConvertToTenThousand(EnergyConsumStructureResult.DataTable[0][1])} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8("气")},{"yLabel",ConvertToTenThousand(EnergyConsumStructureResult.DataTable[0][2])} });

	std::vector<Json> TableDatas;
	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";
	TableData["categoryData"] = CategoryDatas;
	TableDatas.push_back(TableData);

	std::vector<Json> Tables;
	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = TCHAR_TO_UTF8("万tce");
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;

	Data["title"] = TCHAR_TO_UTF8("区域能源消费结构");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AreaEnergyConsumRatio(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> CategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		FMysqlResult RegionSynEnergyConsumResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % ChildRegion[0] % StartTime % EndTime));

		Json CategoryData;
		CategoryData["xLabel"] = ChildRegion[1];
		CategoryData["yLabel"] = ConvertToTenThousand(RegionSynEnergyConsumResult.DataTable[0][0]);
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	TableData["start_color"] = "#8FC31F";
	TableData["end_color"] = "#8FC31F";
	TableData["high_color"] = "#5ceaff";
	TableData["gradient_color"] = "#8FC31F5A";

	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = TCHAR_TO_UTF8("万tce");
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("区域能源消费比例");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AreaEnteEnergyConsumRank(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string TopCount = FIELDIFNULL(InData["TopCount"]);

	std::string EnteTopListCommand = "CALL GetEnteTopListByEnergyConsumption('" + RegionID + "','" + StartTime + "','" + EndTime + "','" + TopCount + "')";
	FMysqlResult EnteTopList = MaraidbConnector.Query(EnteTopListCommand);

	std::vector<Json>CategoryDatas;
	for (auto EnteTopListRow : EnteTopList.DataTable)
		CategoryDatas.push_back({ {"xLabel",EnteTopListRow[1]},{"yLabel",SaveDecimalPlaces(EnteTopListRow[5])} });

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color"] = "#5ceaff";

	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = "tce";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("企业能耗排行");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}
Json OServerHelper::AreaEnteEnergyEfficiencyRank(Json InData)
{
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string TopCount = FIELDIFNULL(InData["TopCount"]);

	std::string EnteTopListCommand = "SELECT EnteName,(SUM(EnergyConsum)/Area)as EnergyConsumUnitArea FROM EnteElecInfo,BaseEnteInfo WHERE BaseEnteInfo.ElecMetID=EnteElecInfo.ElecMetID AND EnteElecInfo.CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY EnteElecInfo.ElecMetID ORDER BY EnergyConsumUnitArea DESC LIMIT " + TopCount + ";";
	FMysqlResult EnteTopList = MaraidbConnector.Query(EnteTopListCommand);

	std::vector<Json>CategoryDatas;
	for (auto EnteTopListRow : EnteTopList.DataTable)
		CategoryDatas.push_back({ {"xLabel",EnteTopListRow[0]},{"yLabel",SaveDecimalPlaces(EnteTopListRow[1])} });

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color"] = "#5ceaff";

	std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("企业单位面积能耗");
	Table["unit"] = "(tce/m²)";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("企业能效排行(单位面积能耗)");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AreaIndustryEnergyConsumRank(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
	FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);

	std::vector<Json> EnergyConsumptionCategoryDatas;

	for (auto ChildIndustryListRow : ChildIndustryList.DataTable)
	{
		std::string IndustryEnergyConsumptionCommand = "SELECT SUM(SynEnergyConsum) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + RegionID + "')) AND IndustryID='" + ChildIndustryListRow[0] + "' AND CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';";
		FMysqlResult IndustryEnergyConsumption = MaraidbConnector.Query(IndustryEnergyConsumptionCommand);

		Json EnergyConsumptionCategoryData;
		EnergyConsumptionCategoryData["xLabel"] = ChildIndustryListRow[1];
		EnergyConsumptionCategoryData["yLabel"] = SaveDecimalPlaces(IndustryEnergyConsumption.DataTable[0][0]);
		EnergyConsumptionCategoryDatas.push_back(EnergyConsumptionCategoryData);
	}

	std::sort(EnergyConsumptionCategoryDatas.begin(), EnergyConsumptionCategoryDatas.end(), SortCategoryDatas);

	Json EnergyConsumptionTableData;
	EnergyConsumptionTableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	EnergyConsumptionTableData["start_color"] = "rgba(96,199,255,0.3)";
	EnergyConsumptionTableData["end_color"] = "rgba(96,199,255,1)";
	EnergyConsumptionTableData["high_color"] = "#5ceaff";
	EnergyConsumptionTableData["categoryData"] = EnergyConsumptionCategoryDatas;

	std::vector<Json>EnergyConsumptionTableDatas;
	EnergyConsumptionTableDatas.push_back(EnergyConsumptionTableData);

	Json EnergyConsumptionTable;
	EnergyConsumptionTable["title"] = TCHAR_TO_UTF8("总");
	EnergyConsumptionTable["unit"] = "tce";
	EnergyConsumptionTable["xLabelRang"] = GetXLabelRangFromCategoryDatas(EnergyConsumptionCategoryDatas);
	EnergyConsumptionTable["data"] = EnergyConsumptionTableDatas;

	std::vector<Json> Tables;
	Tables.push_back(EnergyConsumptionTable);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = TCHAR_TO_UTF8("行业能耗排行");
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::AreaEnergyConsumEfficiencyRank(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string SelectType[] = { "EnergyConsumUnitArea","SynEnergyConsum","EnergyEcnoValueAdd","OutputValue" , "ClearEnergyRation" , "CarbonEmission" };
	std::string Title[] = { TCHAR_TO_UTF8("单位面积能耗"),TCHAR_TO_UTF8("综合能耗"),TCHAR_TO_UTF8("度电经济增加值"),TCHAR_TO_UTF8("万元产值能耗") , TCHAR_TO_UTF8("清洁能源占比") , TCHAR_TO_UTF8("碳排放") };
	std::string Unit[] = { TCHAR_TO_UTF8("tce/km²") , TCHAR_TO_UTF8("万tce") , TCHAR_TO_UTF8("元/kW·h") , TCHAR_TO_UTF8("tce/万元") , "%" , TCHAR_TO_UTF8("t·CO₂") };


	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::string EnergyTypeString = FIELDIFNULL(InData["EnergyType"]);

	int EnergyType = boost::lexical_cast<int>(EnergyTypeString);


	std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='" + RegionID + "';";
	FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);

	std::vector<Json> CategoryDatas;

	for (auto ChildRegion : ChildRegionList.DataTable)
	{
		FMysqlResult RegionEnergyConsumEfficiencyResult = MaraidbConnector.Query(str(boost::format("SELECT SUM(%4%) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' ORDER BY CreateDate;") % ChildRegion[0] % StartTime % EndTime % SelectType[EnergyType]));

		Json CategoryData;
		CategoryData["xLabel"] = ChildRegion[1];
		CategoryData["yLabel"] = EnergyType == 1 ? ConvertToTenThousand(RegionEnergyConsumEfficiencyResult.DataTable[0][0]) : SaveDecimalPlaces(RegionEnergyConsumEfficiencyResult.DataTable[0][0]);
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("类别一");
	TableData["start_color"] = "rgba(96,199,255,0.3)";
	TableData["end_color"] = "rgba(96,199,255,1)";
	TableData["high_color1"] = "#ed5f00";
	TableData["high_color2"] = "#fce700";

	std::sort(CategoryDatas.begin(), CategoryDatas.end(), ( EnergyType >3? SortCategoryDatas:SortCategoryDatasReverse));
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("总");
	Table["unit"] = Unit[EnergyType];
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	std::vector<Json> Datas;

	Data["title"] = Title[EnergyType];
	Data["table"] = Tables;

	Datas.push_back(Data);
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::AreaAbnormalEvents(Json InData)
{
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult AbnormalEventsList = MaraidbConnector.Query(str(boost::format("SELECT ExceptionEventId,AiResult,FilterDate,Status FROM AbnormalEnergyEvent,BaseEnteElecMeterInfo WHERE AbnormalEnergyEvent.ElecMetID=BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(RegionID,GetChildrenRegion('%1%')) AND FilterDate BETWEEN '%2%' AND '%3%' ORDER BY FilterDate DESC LIMIT 30;") % RegionID % StartTime % EndTime));

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"异常事件编号\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"预判原因\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"发生时间\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"状态\"}")));

	std::vector<Json> TableDatas;
	for (auto AbnormalEventsListRow : AbnormalEventsList.DataTable)
	{
		std::vector<Json> Datas;
		for (auto Value : AbnormalEventsListRow)
		{
			Json Data;
			Data["value"] = Value;
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}
	while (TableDatas.size() < 8)
	{
		std::vector<Json> Datas;
		for (auto RowHeaderValue : RowHeader)
		{
			Json Data;
			Data["value"] = "-----";
			Datas.push_back(Data);
		}
		TableDatas.push_back(Datas);
	}

	std::vector<Json> Tables;
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;
	Tables.push_back(Table);

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("最新异常用能事件"));
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json TableData;
	TableData["data"] = Datas;
	return TableData;
}
Json OServerHelper::MarketplaceOverview(Json InData)
{
	using namespace std;
	using namespace boost;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult DemandContractInfo = MaraidbConnector.Query("SELECT SUM(ContractAmount),COUNT(ContractId) FROM DemandContractInfo;");
	FMysqlResult GoodsContractInfo = MaraidbConnector.Query("SELECT SUM(ContractAmount),COUNT(ContractId) FROM GoodsContractInfo;");

	FMysqlResult GoodNum = MaraidbConnector.Query("SELECT COUNT(GoodsId) FROM GoodsInfo WHERE GoodsInfo.GoodsStatus='已上架';");
	FMysqlResult EnteNum = MaraidbConnector.Query("SELECT COUNT(EnteEvalvate.EnteID) FROM EnteEvalvate ;");

	std::vector<Json> Tables;
	Tables.push_back({ {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("交易金额"))},{"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("万元"))},{"yLabel",ConvertToTenThousand(std::to_string(lexical_cast<double>(DemandContractInfo.DataTable[0][0]) + lexical_cast<double>(GoodsContractInfo.DataTable[0][0])))} });
	Tables.push_back({ {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("订单数目"))},{"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("笔"))},{"yLabel",std::to_string((int)(lexical_cast<double>(DemandContractInfo.DataTable[0][1]) + lexical_cast<double>(GoodsContractInfo.DataTable[0][1])))} });
	Tables.push_back({ {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("联盟企业"))},{"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("家"))},{"yLabel",EnteNum.DataTable[0][0]} });
	Tables.push_back({ {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("商品数量"))},{"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("件"))},{"yLabel",GoodNum.DataTable[0][0]} });

	Json Data;
	Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("能源服务市场概况"));
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}
Json OServerHelper::GetBusinessList(Json InData)
{
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	FMysqlResult EnteList = MaraidbConnector.Query(str(boost::format("SELECT ElecMetID,EnteName,Address,ContactPhone,Coordinate FROM BaseEnteInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('%1%'));") % RegionID));
	std::vector<Json> SymbolList;
	for (auto EnteListRow : EnteList.DataTable)
	{
		FVector2D Coordinates = FVector2D();
		std::list<std::string> CoordinatesString;
		boost::split(CoordinatesString, EnteListRow[4], is_any_of(","));
		if (CoordinatesString.size() < 2)continue;
		try
		{
			Coordinates.X = lexical_cast<double>(CoordinatesString.front());
			Coordinates.Y = lexical_cast<double>(CoordinatesString.back());
		}
		catch (const std::exception &)
		{
			continue;
		}

		FVector2D CoordinatesPix = HeatMapHelper::BD09TOPixel(Coordinates);
		Json Symbol;
		Symbol["PositionX"] = CoordinatesPix.X;
		Symbol["PositionY"] = CoordinatesPix.Y;
		Symbol["GUID"] = EnteListRow[0];
		Symbol["Name"] = EnteListRow[1];
		Symbol["Address"] = EnteListRow[2];
		Symbol["Telephone"] = EnteListRow[3];
		SymbolList.push_back(Symbol);
	}
	Json Data;
	Data["SymbolList"] = SymbolList;
	Json ReturnData;
	ReturnData["Data"] = Data;

	return ReturnData;
}

Json OServerHelper::GetRegionBusinessInfo(Json InData)
{
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	FMysqlResult RegionList = MaraidbConnector.Query(str(boost::format("SELECT RegionID,Area FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='%1%';") % RegionID));

	std::vector<Json> SymbolInfo;

	for (auto RegionInfoRow: RegionList.DataTable)
	{
		FMysqlResult RegionEnteCount = MaraidbConnector.Query(str(boost::format("CALL GetRegionInfo_EnteNum('%1%')") % RegionInfoRow[0]));
		
		std::string EnteCount= RegionEnteCount.ResultIsNotNull()? RegionEnteCount.DataTable[0][0]:"0";
		Json SymbolData;
		SymbolData["RegionID"]= RegionInfoRow[0];
		SymbolData["Count"] = EnteCount;
		SymbolData["Area"] = RegionInfoRow[1];

		SymbolInfo.push_back(SymbolData);
	}
	Json Data;
	Data["SymbolInfo"] = SymbolInfo;
	Json ReturnData;
	ReturnData["Data"] = Data;

	return ReturnData;
}

Json OServerHelper::GetPowerStationList(Json InData)
{
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	//std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	FMysqlResult EnteList = MaraidbConnector.Query("SELECT StatElecID,ElecName,Address,Type,Coordinates FROM DistributionFacilties WHERE FIND_IN_SET(RegionID,GetChildrenRegion('zj')) AND Type LIKE 'elecs%'");
	std::vector<Json> SymbolList;
	for (auto EnteListRow : EnteList.DataTable)
	{
		FVector2D Coordinates = FVector2D();
		std::list<std::string> CoordinatesString;
		boost::split(CoordinatesString, EnteListRow[4], is_any_of(","));
		if (CoordinatesString.size() < 2)continue;
		Coordinates.X = lexical_cast<double>(CoordinatesString.front());
		Coordinates.Y = lexical_cast<double>(CoordinatesString.back());
		FVector2D CoordinatesPix = HeatMapHelper::BD09TOPixel(Coordinates);
		Json Symbol;
		Symbol["PositionX"] = CoordinatesPix.X;
		Symbol["PositionY"] = CoordinatesPix.Y;
		Symbol["GUID"] = EnteListRow[0];
		Symbol["Name"] = EnteListRow[1];
		Symbol["Address"] = EnteListRow[2];
		Symbol["Telephone"] = "";
		if (contains(EnteListRow[3], "110"))
		{
			Symbol["Telephone"] = "110kV";
		}
		else if (contains(EnteListRow[3], "220"))
		{
			Symbol["Telephone"] = "220kV";
		}
		else if (contains(EnteListRow[3], "35"))
		{
			Symbol["Telephone"] = "35kV";
		}
		SymbolList.push_back(Symbol);
	}
	Json Data;
	Data["SymbolList"] = SymbolList;
	Json ReturnData;
	ReturnData["Data"] = Data;

	return ReturnData;
}

Json OServerHelper::GetAreaScore(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace boost::gregorian;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	date Now = day_clock::local_day();

	std::string ElecMetID = "1352958966";
	std::string StartTime = to_iso_extended_string(Now - months(1));
	std::string EndTime = to_iso_extended_string(Now);

	std::string RequestAPI = HTTP_URL("/baseInfo/ente/report/num");
	std::string RequsetData = "?startMonth=" + StartTime.substr(0, StartTime.length() - 3) + "&endMonth=" + EndTime.substr(0, EndTime.length() - 3) + "&elecNum=" + ElecMetID;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Tables;
	Tables.push_back({
						 {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("最大设备利用率"))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("选中企业")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUse"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUseIndex"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业标杆")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUseIndex"])))},
						 {"unit","%"}
		});
	Tables.push_back({
						 {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("电量峰谷比"))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("选中企业")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValley"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValleyIndex"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业标杆")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValleyIndex"])))},
						 {"unit",""}
		});
	Tables.push_back({
						 {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("力率奖惩电费"))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("选中企业")),ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCost"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值")),ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCostIndex"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业标杆")),ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCostIndex"])))},
						 {"unit",TCHAR_TO_UTF8(TCHAR_TO_UTF8("万元"))}
		});

	Json Data;
	Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("用能经济性指标"));
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::PendingToAduitOrder(Json InData)
{
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult AccApplyResult = MaraidbConnector.Query(str(boost::format("SELECT OrderCode,ApplyAccName,ApplyEnteName,OfferPrice,CreateDate,ContactPerson FROM AccApply WHERE ApplyStatus='待受理' GROUP BY CreateDate;")));
	FMysqlResult CustApplyResult = MaraidbConnector.Query(str(boost::format("SELECT OrderCode,ApplyServiceName,ApplyEnteName,OfferPrice,CreateDate,ContactPerson FROM CustApply WHERE ApplyStatus='受理中' GROUP BY CreateDate;")));
	FMysqlResult ServerApplyResult = MaraidbConnector.Query(str(boost::format("SELECT OrderCode,OrderName,EnteName,OrderMoney,OrderDate,ContactPerson FROM ServerApply,BaseEnteInfo WHERE ServerApply.EnteID=BaseEnteInfo.EnteID AND OrderStatus='待付款' GROUP BY CreateDate;")));

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"订单编号\",\"width\":\"14\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"服务名称\",\"width\":\"24\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"企业名称\",\"width\":\"26\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"金额\",\"width\":\"12\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"日期\",\"width\":\"12\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"业务负责人\",\"width\":\"12\"}")));

	std::vector<Json> TableDatas;
	for (auto AccApplyResultRow : AccApplyResult.DataTable)
	{
		std::vector<Json> TableData;
		Json Value;
		Value["value"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("数据接入服务"));
		TableData.push_back(Value);
		for (auto AccApplyResultRowValue : AccApplyResultRow)
		{
			Json Value;
			Value["value"] = AccApplyResultRowValue;
			TableData.push_back(Value);
		}
		TableDatas.push_back(TableData);
	}

	for (auto CustApplyResultRow : CustApplyResult.DataTable)
	{
		std::vector<Json> TableData;
		Json Value;
		Value["value"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("定制服务"));
		TableData.push_back(Value);
		for (auto CustApplyResultRowValue : CustApplyResultRow)
		{
			TableData.push_back({ { "value",CustApplyResultRowValue } });
		}
		TableDatas.push_back(TableData);
	}

	for (auto ServerApplyResultRow : ServerApplyResult.DataTable)
	{
		std::vector<Json> TableData;
		Json Value;
		Value["value"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("数据分析服务"));
		TableData.push_back(Value);
		for (auto ServerApplyResultRowValue : ServerApplyResultRow)
		{
			TableData.push_back({ { "value",ServerApplyResultRowValue } });
		}
		TableDatas.push_back(TableData);
	}

	while (TableDatas.size() < 4)
	{
		std::vector<Json> TableData;
		Json Value;
		Value["value"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("全部订单"));
		TableData.push_back(Value);
		for (auto RowValue : RowHeader)
		{
			std::string FillString = "";
			std::string Width = RowValue["width"];
			for (int i = 0; i < lexical_cast<double>(Width) / 2 - 1; i++)
				FillString += "-";
			Json Value;
			Value["value"] = FillString;
			TableData.push_back(Value);
		}
		TableDatas.push_back(TableData);
	}

	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;


	Json Data;
	Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("待处理服务订单"));
	Data["table"] = Table;

	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}
Json OServerHelper::PendingToAduitGoodsAndDemand(Json InData)
{
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult DemandResult = MaraidbConnector.Query(str(boost::format("SELECT DemandInfo.DemandName,DemandInfo.DemandType,DemandInfo.PublishDate,DemandInfo.Budget,DemandInfo.EnteName FROM DemandInfo WHERE FlowStatus='审核中';")));
	FMysqlResult GoodsResult = MaraidbConnector.Query(str(boost::format("SELECT GoodsInfo.GoodsName,GoodsInfo.GoodsType,GoodsInfo.CreateDate,GoodsInfo.GoodsPrice,GoodsInfo.PrvdName FROM GoodsInfo WHERE GoodsStatus='审批中';")));

	std::vector<Json> Datas;
	{
		Json Data;

		std::vector<std::string > RowHeader = { TCHAR_TO_UTF8(TCHAR_TO_UTF8("需求名称")) ,TCHAR_TO_UTF8(TCHAR_TO_UTF8("需求类型")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("发布时间")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("预算金额")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("发布企业")) };
		std::vector<std::string > RowWidth = { "14","24","26","12","24" };

		Json Table = FillTableJson(RowHeader, RowWidth, DemandResult.DataTable);

		std::vector<Json>Tables;
		Tables.push_back(Table);


		Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("需求"));
		Data["table"] = Tables;

		Datas.push_back(Data);
	}

	{
		Json Data;

		std::vector<std::string > RowHeader = { TCHAR_TO_UTF8(TCHAR_TO_UTF8("商品名称")) ,TCHAR_TO_UTF8(TCHAR_TO_UTF8("商品类型")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("发布时间")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("金额")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("发布企业")) };
		std::vector<std::string > RowWidth = { "14","24","26","12","24" };

		Json Table = FillTableJson(RowHeader, RowWidth, GoodsResult.DataTable);
		std::vector<Json>Tables;
		Tables.push_back(Table);

		Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("商品"));
		Data["table"] = Tables;

		Datas.push_back(Data);
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("待审核需求和待审核商品"));
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::WorkResumptionIndex(Json InData)
{
	using namespace cinatra;

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string RequestAPI = HTTP_URL("/rework/region");
	std::string RequsetData = "?startDate=" + StartTime + "&endDate=" + EndTime + "&regionId=zj";
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	if (HttpReceiveInfo["code"] != "-1")
	{
		FXLabelRangCofig XLabelRangCofig = FXLabelRangCofig(1, false, true, true, '.', EXLabelStepDateIterator::Day);
		std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, XLabelRangCofig);

		std::vector<Json> EntePercentCategoryDatas;
		std::vector<Json> EnteIndexCategoryDatas;
		std::vector<Json> ElecPercentCategoryDatas;
		for (auto DataRow : HttpReceiveInfo["data"])
		{
			if ((FIELDIFNULL(DataRow["modelName"])) == TCHAR_TO_UTF8("20200330_复工复产模型v2.4.2.2") && (FIELDIFNULL(DataRow["regionId"])) == "zj")
			{
				boost::gregorian::date CurrentDate = boost::gregorian::from_string(FIELDIFNULL(DataRow["createDate"]));
				std::string  Date = std::to_string(CurrentDate.month()) + "." + std::to_string(CurrentDate.day());
				EntePercentCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",std::to_string(FIELDIFNULL_FLOAT(DataRow["entePercent"]) * 100)} });
				EnteIndexCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",std::to_string(FIELDIFNULL_FLOAT(DataRow["elecIndex"]))} });
				ElecPercentCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",std::to_string(FIELDIFNULL_FLOAT(DataRow["elecPercent"]) * 100)} });
			}
		}

		std::vector<Json> CategoryDatas[3] = { EntePercentCategoryDatas ,EnteIndexCategoryDatas ,ElecPercentCategoryDatas };
		std::string CategoryNames[3] = { TCHAR_TO_UTF8(TCHAR_TO_UTF8("复工企业占比")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("复工电力指数")),TCHAR_TO_UTF8(TCHAR_TO_UTF8("复工电量占比")) };
		std::string Color[3] = { "rgba(65,141,255,1)","rgba(0,254,233,1)","rgba(255,255,255,1)" };

		std::vector<Json> TableDatas;
		for (int i = 0; i < 3; i++)
		{
			Json TableData;
			TableData["categoryName"] = CategoryNames[i];
			TableData["start_color"] = Color[i];
			TableData["end_color"] = Color[i];
			TableData["high_color"] = Color[i];
			TableData["gradient_color"] = Color[i];
			TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas[i]);
			TableDatas.push_back(TableData);
		}

		Json Table;
		Table["title"] = "";
		Table["unit"] = "%";
		Table["xLabelRang"] = XLabelRangs;
		Table["data"] = TableDatas;

		std::vector<Json> Tables;
		Tables.push_back(Table);

		Json Data;
		Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("复工电力指数"));
		Data["table"] = Tables;

		std::vector<Json> Datas;
		Datas.push_back(Data);

		Json ReturnData;
		ReturnData["data"] = Datas;

		return ReturnData;
	}

	return {};

}
Json OServerHelper::HighNetWorthRatio(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionEnteCountResult = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ElecMetID) FROM BaseEnteElecMeterInfo WHERE FIND_IN_SET(BaseEnteElecMeterInfo.RegionID,GetChildrenRegion('%1%'));") % RegionID));

	std::string RequestAPI = HTTP_URL("/baseInfo/high/worth/user");
	std::string RequsetData = "?startYear=" + StartTime + "&endYear=" + EndTime + "&regionId=" + RegionID;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	int KeyUserI = 0;
	int KeyUserII = 0;
	int GeneralLargeUsers = 0;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		KeyUserI += DataRow["keyUsersI"].is_null() ? 0 : DataRow["keyUsersI"].get<int>();
		KeyUserII += DataRow["keyUsersII"].is_null() ? 0 : DataRow["keyUsersII"].get<int>();
		GeneralLargeUsers += DataRow["generalLargeUsers"].is_null() ? 0 : DataRow["generalLargeUsers"].get<int>();
	}
	int NormalUser = lexical_cast<int>(RegionEnteCountResult.DataTable[0][0]) - GeneralLargeUsers - KeyUserI - KeyUserII;

	std::vector<Json> Datas;
	Datas.push_back({ {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("普通用户"))},{"value",std::to_string(NormalUser)} });
	Datas.push_back({ {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("高净值用户"))},{"value",std::to_string(GeneralLargeUsers + KeyUserI + KeyUserII)} });
	Datas.push_back({ {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("一般大用户"))},{"value",std::to_string(GeneralLargeUsers)} });
	Datas.push_back({ {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("关键用户"))},{"value",std::to_string(KeyUserI + KeyUserII)} });

	Json ReturnData;

	ReturnData["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("高净值用户构成"));
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::ElecEconomyIndex(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string RequestAPI = HTTP_URL("/baseInfo/ente/report/num");
	std::string RequsetData = "?startMonth=" + StartTime.substr(0, StartTime.length() - 3) + "&endMonth=" + EndTime.substr(0, EndTime.length() - 3) + "&elecNum=" + ElecMetID;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Tables;
	Tables.push_back({
						 {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("最大设备利用率"))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("选中企业")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUse"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUseIndex"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业标杆")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUseIndex"])))},
						 {"unit","%"}
		});
	Tables.push_back({
						 {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("电量峰谷比"))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("选中企业")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValley"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValleyIndex"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业标杆")),SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValleyIndex"])))},
						 {"unit",""}
		});
	Tables.push_back({
						 {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("力率奖惩电费"))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("选中企业")),ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCost"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值")),ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCostIndex"])))},
						 {TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业标杆")),ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCostIndex"])))},
						 {"unit",TCHAR_TO_UTF8(TCHAR_TO_UTF8("万元"))}
		});

	Json Data;
	Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("用能经济性指标"));
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::ElecEconomyIndex_AIIsland(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = "1352958966";
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string RequestAPI = HTTP_URL("/baseInfo/ente/report/num");
	std::string RequsetData = "?startMonth=" + StartTime.substr(0, StartTime.length() - 3) + "&endMonth=" + EndTime.substr(0, EndTime.length() - 3) + "&elecNum=" + ElecMetID;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	Json CategoryDatas;
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("最大设备利用率"))},
							{"y1Label",SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUseIndex"])))},
							{"y2Label",SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["deviceUse"])))} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("电量峰谷比"))},
							{"y1Label",SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValleyIndex"])))},
							{"y2Label",SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["peakValley"])))} });
	CategoryDatas.push_back({ {"xLabel",TCHAR_TO_UTF8(TCHAR_TO_UTF8("力率奖惩电费"))},
							{"y1Label",SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCostIndex"])))},
							{"y2Label",SaveDecimalPlaces(std::to_string(FIELDIFNULL_FLOAT(HttpReceiveInfo["data"][0]["rewardCost"])))} });

	Json TableData;
	TableData["categoryName"] = "";
	TableData["start_color"] = "rgba(1,55,122,1)";
	TableData["end_color"] = "rgba(92,234,255,1)";
	TableData["high_color1"] = "#ed5f00";
	TableData["high_color2"] = "#fce700";
	TableData["categoryData"] = CategoryDatas;

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("用电经济性指标同行对比"));
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::PercentageTransactionCategories(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::vector<Json> Tables;

	{
		std::string GoodsSaleInfoCommand = "SELECT GoodsType,COUNT(GoodsContractInfo.GoodsID) FROM GoodsContractInfo,GoodsInfo WHERE GoodsContractInfo.GoodsId=GoodsInfo.GoodsID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY GoodsType ORDER BY COUNT(GoodsContractInfo.GoodsID);";
		FMysqlResult GoodsSaleInfo = MaraidbConnector.Query(GoodsSaleInfoCommand);

		std::vector<Json> CategoryDatas;
		for (auto GoodsSaleInfoRow : GoodsSaleInfo.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = GoodsSaleInfoRow[0];
			CategoryData["yLabel"] = GoodsSaleInfoRow[1];
			CategoryDatas.push_back(CategoryData);
		}
		std::sort(CategoryDatas.begin(), CategoryDatas.end(), SortCategoryDatas);

		std::vector<Json>TableDatas;
		Json TableData;
		TableData["categoryName"] = "";
		TableData["start_color"] = "#00FDAD";
		TableData["end_color"] = "#00FDAD";
		TableData["high_color"] = "#00FDAD";
		TableData["gradient_color"] = "#00FDAD";
		TableData["categoryData"] = CategoryDatas;
		TableDatas.push_back(TableData);

		Json Table;
		Table["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("商品"));
		Table["unit"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("个"));
		Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
		Table["data"] = TableDatas;
		Tables.push_back(Table);
	}

	{
		std::string DemmandCountCommand = "SELECT DemandType,COUNT(DemandContractInfo.DemandID) FROM DemandContractInfo,DemandInfo WHERE DemandContractInfo.DemandId=DemandInfo.DemandID AND SignDate BETWEEN '" + StartTime + "' AND '" + EndTime + "' GROUP BY DemandType ORDER BY COUNT(DemandContractInfo.DemandID);";
		FMysqlResult DemandCount = MaraidbConnector.Query(DemmandCountCommand);

		std::vector<Json> CategoryDatas;
		for (auto DemandCountRow : DemandCount.DataTable)
		{
			Json CategoryData;
			CategoryData["xLabel"] = DemandCountRow[0];
			CategoryData["yLabel"] = DemandCountRow[1];
			CategoryDatas.push_back(CategoryData);
		}

		std::vector<Json> TableDatas;
		Json TableData;
		TableData["categoryName"] = "";
		TableData["start_color"] = "#00FDAD";
		TableData["end_color"] = "#00FDAD";
		TableData["high_color"] = "#00FDAD";
		TableData["gradient_color"] = "#00FDAD";
		TableData["categoryData"] = CategoryDatas;
		TableDatas.push_back(TableData);

		Json Table;
		Table["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("需求"));
		Table["unit"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("个"));
		Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(CategoryDatas);
		Table["data"] = TableDatas;
		Tables.push_back(Table);

	}

	std::vector<Json>Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("近30天成交分类统计");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::MainPageEnteInfo(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EnteInfoResult = MaraidbConnector.Query(str(boost::format("SELECT EnteShortName,Area,EnteName FROM BaseEnteInfo WHERE ElecMetID='%1%';") % ElecMetID));

	std::string EnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption_SUM('" + ElecMetID + "','" + StartTime + "','" + EndTime + "')";
	FMysqlResult EnteEnergyConsumption = MaraidbConnector.Query(EnteEnergyConsumptionCommand);
	FMysqlResult EnteOutputValue = MaraidbConnector.Query(str(boost::format("SELECT SUM(OutputValue) FROM EnteOutputValue WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
	FMysqlResult EnteConsumUnitArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(AreaTce) FROM EnteEnergyConsumUnitArea WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
	FMysqlResult EnteCarbonEmission = MaraidbConnector.Query(str(boost::format("SELECT SUM(EnergyConsum) FROM EnteCarbonEmission WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));

	FMysqlResult EnteIndustryRegionInfo = MaraidbConnector.Query("SELECT IndustryID,RegionID FROM BaseEnteElecMeterInfo WHERE BaseEnteElecMeterInfo.ElecMetID='" + ElecMetID + "';");
	if (!EnteIndustryRegionInfo.ResultIsNotNull()) return {};

	FMysqlResult IndustryRegionEnteCount = MaraidbConnector.Query("SELECT COUNT(ElecMetID) FROM BaseEnteElecMeterInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('" + EnteIndustryRegionInfo.DataTable[0][1] + "')) AND IndustryID='" + EnteIndustryRegionInfo.DataTable[0][0] + "';");
	FMysqlResult IndustryEnergyEffiency = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum),SUM(OutputValue),SUM(EnergyConsumUnitArea),SUM(CarbonEmission) FROM IndustryEnergyEfficiencyMonth,BaseEnteElecMeterInfo WHERE IndustryEnergyEfficiencyMonth.IndustryID=BaseEnteElecMeterInfo.IndustryID AND FIND_IN_SET(IndustryEnergyEfficiencyMonth.RegionID,GetChildrenRegion(BaseEnteElecMeterInfo.RegionID)) AND BaseEnteElecMeterInfo.ElecMetID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));

	std::vector<Json> Datas;

	Datas.push_back({
				   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("企业名称"))},
				   {"content",{{
						 {"key",EnteInfoResult.DataTable[0][2]},
						 {"value",""}
				   }}}
		});

	Datas.push_back({
					   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("企业简称"))},
					   {"content",{{
							 {"key",EnteInfoResult.DataTable[0][0]},
							 {"value",""}
					   }}}
		});

	Datas.push_back({
				   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("总能耗"))},
				   {"content",
					   {
						   {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("指标值"))},
								 {"value", SaveDecimalPlaces(EnteEnergyConsumption.DataTable[0][3]) + " tce"}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值"))},
								 {"value", SaveDecimalPlaces(std::to_string(lexical_cast<double>(ConvertEmptyStringToZero(IndustryEnergyEffiency.DataTable[0][0])) / lexical_cast<double>(ConvertEmptyStringToZero(IndustryRegionEnteCount.DataTable[0][0])))) + " tce"}
						   }
					   }
				   }
		});

	Datas.push_back({
				   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("万元产值能耗"))},
				   {"content",
					   {
						   {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("指标值"))},
								 {"value",FillPlaceholderIfNull(SaveDecimalPlaces(EnteOutputValue.DataTable[0][0])) + TCHAR_TO_UTF8(" tce/万元")}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值"))},
								 {"value",FillPlaceholderIfNull(SaveDecimalPlaces(std::to_string(lexical_cast<double>(ConvertEmptyStringToZero(IndustryEnergyEffiency.DataTable[0][1])) / lexical_cast<double>(ConvertEmptyStringToZero(IndustryRegionEnteCount.DataTable[0][0]))))) + TCHAR_TO_UTF8(" tce/万元")}
						   }
					   }
				   }
		});

	Datas.push_back({
				   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("单位建筑面积能耗"))},
				   {"content",
					   {
						   {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("指标值"))},
								 {"value",FillPlaceholderIfNull(GetEnergyConsumptionUnitArea(EnteEnergyConsumption.DataTable[0][3],EnteInfoResult.DataTable[0][1])) + TCHAR_TO_UTF8(" tce/m²")}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值"))},
								 {"value",FillPlaceholderIfNull(SaveDecimalPlaces(std::to_string(lexical_cast<double>(ConvertEmptyStringToZero(IndustryEnergyEffiency.DataTable[0][2])) / lexical_cast<double>(ConvertEmptyStringToZero(IndustryRegionEnteCount.DataTable[0][0]))))) + TCHAR_TO_UTF8(" tce/m²")}
						   }
					   }
				   }
		});

	Datas.push_back({
				   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("碳排放"))},
				   {"content",
					   {
						   {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("指标值"))},
								 {"value",FillPlaceholderIfNull(SaveDecimalPlaces(EnteCarbonEmission.DataTable[0][0])) + TCHAR_TO_UTF8(" t·CO₂")}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("行业均值"))},
								 {"value",FillPlaceholderIfNull(SaveDecimalPlaces(std::to_string(lexical_cast<double>(ConvertEmptyStringToZero(IndustryEnergyEffiency.DataTable[0][3])) / lexical_cast<double>(ConvertEmptyStringToZero(IndustryRegionEnteCount.DataTable[0][0]))))) + TCHAR_TO_UTF8(" t·CO₂")}
						   }
					   }
				   }
		});


	std::string RequestAPI = HTTP_URL("/baseInfo/ente/energy/expenditure");
	std::string StartMonth = StartTime.substr(0, StartTime.length() - 3);
	std::string EndMonth = EndTime.substr(0, StartTime.length() - 3);
	std::string RequsetData = "?startMonth=" + StartMonth + "&endMonth=" + EndMonth + "&elecNum=" + ElecMetID;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};

	Json ResultData = Json::parse(Result);
	std::string FullCost = ConvertToTenThousand(FIELDIFNULL(ResultData["data"][0]["valueCostTotal"]));
	std::string ElecCost = FullCost;

	//	std::string ElecCost = ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(ResultData["data"][0]["elecCost"])));
	std::string WaterCost = ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(ResultData["data"][0]["waterCost"])));
	std::string GasCost = ConvertToTenThousand(std::to_string(FIELDIFNULL_FLOAT(ResultData["data"][0]["gasCost"])));


	Datas.push_back({
				   {"title",TCHAR_TO_UTF8(TCHAR_TO_UTF8("用能支出"))},
				   {"content",
					   {
						   {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("总费用"))},
								 {"value",FillPlaceholderIfNull(FullCost) + TCHAR_TO_UTF8(" 万元")}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("电费"))},
								 {"value",FillPlaceholderIfNull(ElecCost) + TCHAR_TO_UTF8(" 万元")}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("气费"))},
								 {"value",FillPlaceholderIfNull(GasCost) + TCHAR_TO_UTF8(" 万元")}
						   },
						  {
								 {"key",TCHAR_TO_UTF8(TCHAR_TO_UTF8("水费"))},
								 {"value",FillPlaceholderIfNull(WaterCost) + TCHAR_TO_UTF8(" 万元")}
						   }
					   }
				   }
		});

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8(TCHAR_TO_UTF8("企业基本信息"));
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::EnteEnergyEfficiencyTrend(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ElecMetID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, Config);

	FMysqlResult EnteNameResult = MaraidbConnector.Query(str(boost::format("SELECT EnteName FROM BaseEnteInfo WHERE ElecMetID='%1%';") % ElecMetID));

	std::string EnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption('" + ElecMetID + "','" + StartTime + "','" + EndTime + "')";
	FMysqlResult EnteEnergyConsumption = MaraidbConnector.Query(EnteEnergyConsumptionCommand);

	FMysqlResult EnteOutputValue = MaraidbConnector.Query(str(boost::format("SELECT CreateDate, OutputValue FROM EnteOutputValue WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
	FMysqlResult EnteConsumUnitArea = MaraidbConnector.Query(str(boost::format("SELECT CreateDate, AreaTce FROM EnteEnergyConsumUnitArea WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
	FMysqlResult EnteEnergyEcnoValueAdd = MaraidbConnector.Query(str(boost::format("SELECT CreateDate, EcnoValueAdd FROM EnteEnergyEcnoValueAdd WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));

	std::vector<Json> ChartDatas;

	{
		std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
		std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };

		std::vector<Json> Datas;

		for (int Index = 0; Index < 4; Index++)
		{
			std::vector<Json>CategoryDatas;
			for (auto EnteEnergyConsumptionRow : EnteEnergyConsumption.DataTable)
				CategoryDatas.push_back({
										 {"xLabel",ConvertDateToMonth(EnteEnergyConsumptionRow[0])},
										 {"yLabel",EnteEnergyConsumptionRow[Index == 0 ? 1 : Index + 4]}
					});

			std::vector<Json> TableDatas;
			Json TableData;
			TableData["categoryName"] = "";
			TableData["start_color"] = "rgba(65,141,255,0)";
			TableData["end_color"] = "rgba(65,141,255,1)";
			TableData["high_color"] = "#5ceaff";
			TableData["gradient_color"] = "#8FC31F";

			TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
			TableDatas.push_back(TableData);

			Json Data;
			Data["title"] = Title[Index];
			Data["unit"] = Unit[Index];
			Data["xLabelRang"] = XLabelRangs;
			Data["data"] = TableDatas;

			Datas.push_back(Data);
		}
		Json ChartData;
		ChartData["title"] = TCHAR_TO_UTF8("能耗趋势");
		ChartData["data"] = Datas;

		ChartDatas.push_back(ChartData);
	}


	{
		std::string Title[3] = { TCHAR_TO_UTF8("单位面积能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("度电经济增加值") };
		std::string Unit[3] = { TCHAR_TO_UTF8("tce/km²"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("万元/kW·h") };
		FMysqlResult Result[3] = { EnteConsumUnitArea,EnteOutputValue,EnteEnergyEcnoValueAdd };

		for (int Index = 0; Index < 3; Index++)
		{
			std::vector<Json>CategoryDatas;
			for (auto ResultRow : Result[Index].DataTable)
				CategoryDatas.push_back({
										 {"xLabel",ConvertDateToMonth(ResultRow[0])},
										 {"yLabel",ResultRow[1]}
					});

			std::vector<Json> TableDatas;
			Json TableData;
			TableData["categoryName"] = "";
			TableData["start_color"] = "rgba(65,141,255,0)";
			TableData["end_color"] = "rgba(65,141,255,1)";
			TableData["high_color"] = "#5ceaff";
			TableData["gradient_color"] = "#8FC31F";
			TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
			TableDatas.push_back(TableData);

			std::vector<Json> Datas;
			Json Data;
			Data["title"] = "";
			Data["unit"] = Unit[Index];
			Data["xLabelRang"] = XLabelRangs;
			Data["data"] = TableDatas;

			Datas.push_back(Data);

			Json ChartData;
			ChartData["title"] = Title[Index];
			ChartData["data"] = Datas;

			ChartDatas.push_back(ChartData);
		}
	}

	Json ReturnData;
	ReturnData["chartName"] = TCHAR_TO_UTF8("企业用能变化趋势");
	ReturnData["chartData"] = ChartDatas;

	return ReturnData;
}
Json OServerHelper::RegionIndustryEnteList(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	date D_StartTime = from_string(StartTime);
	date D_EndTime = from_string(EndTime);

	date D_PreYearStartTime = D_StartTime - years(1);
	date D_PreYearEndTime = D_EndTime - years(1);

	std::vector<Json> Tables;

	FMysqlResult CurrentYearRegionList = MaraidbConnector.Query(str(boost::format("SELECT RegionEnergyEfficiencyMonth.RegionID,RegionName,SUM(SynEnergyConsum) FROM RegionEnergyEfficiencyMonth LEFT JOIN BaseRegionInfo ON BaseRegionInfo.RegionID=RegionEnergyEfficiencyMonth.RegionID WHERE RegionEnergyEfficiencyMonth.RegionID=BaseRegionInfo.RegionID AND BaseRegionInfo.ParentID='%3%' AND CreateDate BETWEEN '%1%' AND '%2%' GROUP BY RegionID;") % StartTime % EndTime % RegionID));
	std::vector<Json> RegionTableDatas;
	for (auto CurrentYearRegionListRow : CurrentYearRegionList.DataTable)
	{
		FMysqlResult PreYearConsumption = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum) FROM RegionEnergyEfficiencyMonth LEFT JOIN BaseRegionInfo ON BaseRegionInfo.RegionID=RegionEnergyEfficiencyMonth.RegionID WHERE RegionEnergyEfficiencyMonth.RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % CurrentYearRegionListRow[0] % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));

		Json TableData;
		TableData["id"] = CurrentYearRegionListRow[0];
		TableData["name"] = CurrentYearRegionListRow[1];
		TableData["value"] = "-----";
		if (PreYearConsumption.DataTable.size())
		{
			if (PreYearConsumption.DataTable[0][0] != "0" && PreYearConsumption.DataTable[0][0] != "")
			{
				double Ratio = (lexical_cast<double>(CurrentYearRegionListRow[2]) / lexical_cast<double>(PreYearConsumption.DataTable[0][0])) - 1;
				TableData["value"] = SaveDecimalPlaces(std::to_string(Ratio * 100)) + "%";
			}
		}
		RegionTableDatas.push_back(TableData);
	}
	Json RegionTable;
	RegionTable["title"] = TCHAR_TO_UTF8("区域");
	RegionTable["data"] = RegionTableDatas;
	Tables.push_back(RegionTable);

	FMysqlResult CurrentYearIndustryList = MaraidbConnector.Query(str(boost::format("SELECT IndustryEnergyEfficiencyMonth.IndustryID,IndustryName,SUM(SynEnergyConsum) FROM BaseRegionInfo,IndustryEnergyEfficiencyMonth left JOIN BaseIndustryInfo ON BaseIndustryInfo.IndustryID=IndustryEnergyEfficiencyMonth.IndustryID WHERE IndustryEnergyEfficiencyMonth.RegionID=BaseRegionInfo.RegionID AND BaseRegionInfo.ParentID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY IndustryID ;") % RegionID % StartTime % EndTime));

	std::vector<Json> IndustryTableDatas;
	for (auto CurrentYearIndustryListRow : CurrentYearIndustryList.DataTable)
	{
		FMysqlResult PreYearConsumption = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum) FROM BaseRegionInfo,IndustryEnergyEfficiencyMonth left JOIN BaseIndustryInfo ON BaseIndustryInfo.IndustryID=IndustryEnergyEfficiencyMonth.IndustryID WHERE IndustryEnergyEfficiencyMonth.RegionID=BaseRegionInfo.RegionID AND BaseRegionInfo.ParentID='%4%' AND IndustryEnergyEfficiencyMonth.IndustryID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % CurrentYearIndustryListRow[0] % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)% RegionID));

		Json TableData;
		TableData["id"] = CurrentYearIndustryListRow[0];
		TableData["name"] = CurrentYearIndustryListRow[1];
		TableData["value"] = "-----";
		if (PreYearConsumption.DataTable.size())
		{
			if (PreYearConsumption.DataTable[0][0] != "0" && PreYearConsumption.DataTable[0][0] != "")
			{
				double Ratio = (lexical_cast<double>(CurrentYearIndustryListRow[2]) / lexical_cast<double>(PreYearConsumption.DataTable[0][0])) - 1;
				TableData["value"] = SaveDecimalPlaces(std::to_string(Ratio * 100)) + "%";
			}
		}
		IndustryTableDatas.push_back(TableData);
	}
	Json IndustryTable;
	IndustryTable["title"] = TCHAR_TO_UTF8("行业");
	IndustryTable["data"] = IndustryTableDatas;
	Tables.push_back(IndustryTable);

	FMysqlResult CurrentYearEnteList = MaraidbConnector.Query(str(boost::format("CALL GetEnteTopListByEnergyConsumption('%3%','%1%','%2%','20')") % StartTime % EndTime % RegionID));
	std::vector<Json> EnteTableDatas;
	for (auto CurrentYearEnteListRow : CurrentYearEnteList.DataTable)
	{
		FMysqlResult PreYearConsumption = MaraidbConnector.Query(str(boost::format("CALL GetEnteEnergyConsumption_SUM('%1%','%2%','%3%')") % CurrentYearEnteListRow[0] % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));


		Json TableData;
		TableData["id"] = CurrentYearEnteListRow[0];
		TableData["name"] = CurrentYearEnteListRow[1];
		TableData["value"] = "-----";
		if (PreYearConsumption.DataTable.size())
		{
			if (PreYearConsumption.DataTable[0][3] != "0" && PreYearConsumption.DataTable[0][3] != "")
			{
				double Ratio = (lexical_cast<double>(CurrentYearEnteListRow[5]) / lexical_cast<double>(PreYearConsumption.DataTable[0][3])) - 1;
				TableData["value"] = SaveDecimalPlaces(std::to_string(Ratio * 100)) + "%";
			}
		}
		EnteTableDatas.push_back(TableData);
	}
	Json EnteTable;
	EnteTable["title"] = TCHAR_TO_UTF8("企业");
	EnteTable["data"] = EnteTableDatas;
	Tables.push_back(EnteTable);

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("区域、行业及重点企业用能分析");
	ReturnData["tables"] = Tables;

	return ReturnData;

}
Json OServerHelper::EnergyEfficiencyAnalysis(Json InData)
{
	std::string Type = FIELDIFNULL(InData["Type"]);
	if (Type == TCHAR_TO_UTF8("区域"))
	{
		return RegionEnergyEfficiencyAnalysis(InData);
	}
	if (Type == TCHAR_TO_UTF8("行业"))
	{
		return IndustryEnergyEfficiencyAnalysis(InData);
	}
	if (Type == TCHAR_TO_UTF8("企业"))
	{
		return EnteEnergyEfficiencyAnalysis(InData);
	}
	return {};
}
Json OServerHelper::IndustryEnergyEfficiencyAnalysis(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string IndustryID = FIELDIFNULL(InData["ID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	date D_StartTime = from_string(StartTime);
	date D_EndTime = from_string(EndTime);

	date D_PreYearStartTime = D_StartTime - years(1);
	date D_PreYearEndTime = D_EndTime - years(1);

	FXLabelRangCofig MonthConfig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	std::vector<Json> MonthXLabelRangs = GetXLabelRang(StartTime, EndTime, MonthConfig);

	FXLabelRangCofig MonthOnlyConfig = FXLabelRangCofig(1, false, true, false, '-', EXLabelStepDateIterator::Month);

	std::vector<Json> MonthOnlyXLabelRangs = GetXLabelRang(StartTime, EndTime, MonthOnlyConfig);

	std::vector<Json> Datas;

	{
		std::vector<Json> ChartDatas;

		//Year
		{
			date D_YearStartTime = from_string(YearStartTime);
			std::vector<Json> CategoryDatas[4];
			std::vector<Json> YearXLabelRangs;
			while (D_YearStartTime < D_EndTime)
			{
				date SelectStartDate = D_YearStartTime;
				D_YearStartTime = D_YearStartTime + years(1);
				FMysqlResult MonthIndustryEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum),SUM(ElecConsumSelf),SUM(WaterConsumSelf),SUM(GasConsumSelf) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('zj')) AND IndustryID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % IndustryID % to_iso_extended_string(SelectStartDate) % to_iso_extended_string(D_YearStartTime - days(1))));

				Json XLabelRang;
				XLabelRang["label"] = std::to_string(SelectStartDate.year());
				YearXLabelRangs.push_back(XLabelRang);

				for (int Index = 0; Index < 4; Index++)
					CategoryDatas[Index].push_back({ {"xLabel",std::to_string(SelectStartDate.year())},{"yLabel",SaveDecimalPlaces(MonthIndustryEnergyConsumption.DataTable[0][Index],5)} });
			}
			std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
			std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };

			std::vector<Json> Tables;
			for (int Index = 0; Index < 4; Index++)
			{
				std::vector<Json> TableDatas;
				Json TableData;
				TableData["categoryName"] = "";
				TableData["start_color"] = "rgba(65,141,255,1)";
				TableData["end_color"] = "rgba(0,254,233,1)";
				TableData["high_color"] = "#5ceaff";
				TableData["gradient_color"] = "#8FC31F";
				TableData["categoryData"] = CategoryDatas[Index];
				TableDatas.push_back(TableData);

				Json Table;
				Table["title"] = Title[Index];
				Table["unit"] = Unit[Index];
				Table["xLabelRang"] = YearXLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);
			}

			Json ChartData;
			ChartData["title"] = TCHAR_TO_UTF8("年");
			ChartData["type"] = "line";
			ChartData["table"] = Tables;

			ChartDatas.push_back(ChartData);
		}

		//Month
		{
			FMysqlResult CurrIndustrynteEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,SUM(SynEnergyConsum),SUM(ElecConsumSelf),SUM(WaterConsumSelf),SUM(GasConsumSelf) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('zj')) AND IndustryID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY CreateDate ORDER BY CreateDate;") % IndustryID % StartTime % EndTime));
			FMysqlResult PreYearIndustryEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,SUM(SynEnergyConsum),SUM(ElecConsumSelf),SUM(WaterConsumSelf),SUM(GasConsumSelf) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('zj')) AND IndustryID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY CreateDate ORDER BY CreateDate;") % IndustryID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));

			FMysqlResult IndustryEnergyConsumption[2] = { CurrIndustrynteEnergyConsumption ,PreYearIndustryEnergyConsumption };
			{
				std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
				std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };
				std::string CategoryName[2] = { TCHAR_TO_UTF8("本年度能耗"),TCHAR_TO_UTF8("去年同期能耗") };
				std::string StartColor[2] = { "rgba(133,222,255,0)","rgba(255,255,255,0)" };
				std::string EndColor[2] = { "rgba(133,222,255,1)","rgba(255,255,255,1)" };
				std::string HighColor[2] = { "#5ceaff","#5ceaff" };
				std::string GradientColor[2] = { "#8FC31F","#8FC31F" };

				std::vector<Json> Tables;

				for (int Index = 0; Index < 4; Index++)
				{
					std::vector<Json> TableDatas;

					for (int CategoryIndex = 0; CategoryIndex < 2; CategoryIndex++)
					{
						std::vector<Json>CategoryDatas;
						for (auto IndustryEnergyConsumptionRow : IndustryEnergyConsumption[CategoryIndex].DataTable)
							CategoryDatas.push_back({
													 {"xLabel",ConvertDateToMonthOnly(IndustryEnergyConsumptionRow[0])},
													 {"yLabel",SaveDecimalPlaces(IndustryEnergyConsumptionRow[1 + Index],5)}
								});

						Json TableData;
						TableData["categoryName"] = CategoryName[CategoryIndex];
						TableData["start_color"] = StartColor[CategoryIndex];
						TableData["end_color"] = EndColor[CategoryIndex];
						TableData["high_color"] = HighColor[CategoryIndex];
						TableData["gradient_color"] = GradientColor[CategoryIndex];
						TableData["categoryData"] = FillCategoryData(MonthOnlyXLabelRangs, CategoryDatas);
						TableDatas.push_back(TableData);
					}
					Json Table;
					Table["title"] = Title[Index];
					Table["unit"] = Unit[Index];
					Table["xLabelRang"] = MonthOnlyXLabelRangs;
					Table["data"] = TableDatas;

					Tables.push_back(Table);
				}
				Json ChartData;
				ChartData["title"] = TCHAR_TO_UTF8("月");
				ChartData["type"] = "bar";
				ChartData["table"] = Tables;

				ChartDatas.push_back(ChartData);
			}
		}

		Json Data;
		Data["chartName"] = TCHAR_TO_UTF8("能耗趋势");
		Data["chartData"] = ChartDatas;
		Datas.push_back(Data);
	}

	{
		FMysqlResult IndustryEnergyEfficiency = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,SUM(EnergyConsumUnitArea),SUM(OutputValue),AVG(EnergyEcnoValueAdd) FROM IndustryEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('zj')) AND IndustryID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY CreateDate ORDER BY CreateDate;") % IndustryID % StartTime % EndTime));

		{
			std::string Title[3] = { TCHAR_TO_UTF8("单位建筑面积能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("度电经济增加值") };
			std::string Unit[3] = { TCHAR_TO_UTF8("tce/km²"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("万元/kW·h") };
			for (int Index = 0; Index < 3; Index++)
			{
				std::vector<Json>ChartDatas;

				std::vector<Json>CategoryDatas;
				for (auto ResultRow : IndustryEnergyEfficiency.DataTable)
					CategoryDatas.push_back({
											 {"xLabel",ConvertDateToMonth(ResultRow[0])},
											 {"yLabel",SaveDecimalPlaces(ResultRow[1 + Index],5)}
						});

				std::vector<Json> TableDatas;
				Json TableData;
				TableData["categoryName"] = "";
				TableData["start_color"] = "rgba(65,141,255,0)";
				TableData["end_color"] = "rgba(65,141,255,1)";
				TableData["high_color"] = "#5ceaff";
				TableData["gradient_color"] = "#8FC31F";
				TableData["categoryData"] = FillCategoryData(MonthXLabelRangs, CategoryDatas);
				TableDatas.push_back(TableData);

				std::vector<Json> Tables;
				Json Table;
				Table["title"] = "";
				Table["unit"] = Unit[Index];
				Table["xLabelRang"] = MonthXLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);

				Json ChartData;
				ChartData["title"] = "";
				ChartData["type"] = "dataZoom";
				ChartData["table"] = Tables;

				ChartDatas.push_back(ChartData);

				Json Data;
				Data["chartName"] = Title[Index];
				Data["chartData"] = ChartDatas;
				Datas.push_back(Data);
			}
		}

	}
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::EnteEnergyEfficiencyAnalysis(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string ElecMetID = FIELDIFNULL(InData["ID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	date D_StartTime = from_string(StartTime);
	date D_EndTime = from_string(EndTime);

	date D_PreYearStartTime = D_StartTime - years(1);
	date D_PreYearEndTime = D_EndTime - years(1);

	FXLabelRangCofig MonthConfig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	std::vector<Json> MonthXLabelRangs = GetXLabelRang(StartTime, EndTime, MonthConfig);

	FXLabelRangCofig MonthOnlyConfig = FXLabelRangCofig(1, false, true, false, '-', EXLabelStepDateIterator::Month);

	std::vector<Json> MonthOnlyXLabelRangs = GetXLabelRang(StartTime, EndTime, MonthOnlyConfig);

	std::vector<Json> Datas;

	{
		std::vector<Json> ChartDatas;

		//Year
		{
			date D_YearStartTime = from_string(YearStartTime);
			std::vector<Json> CategoryDatas[4];
			std::vector<Json> YearXLabelRangs;
			while (D_YearStartTime < D_EndTime)
			{
				date SelectStartDate = D_YearStartTime;
				D_YearStartTime = D_YearStartTime + years(1);
				std::string MonthEnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption_SUM('" + ElecMetID + "','" + to_iso_extended_string(SelectStartDate) + "','" + to_iso_extended_string(D_YearStartTime - days(1)) + "')";
				FMysqlResult MonthEnteEnergyConsumption = MaraidbConnector.Query(MonthEnteEnergyConsumptionCommand);

				Json XLabelRang;
				XLabelRang["label"] = std::to_string(SelectStartDate.year());
				YearXLabelRangs.push_back(XLabelRang);

				for (int Index = 0; Index < 4; Index++)
					CategoryDatas[Index].push_back({ {"xLabel",std::to_string(SelectStartDate.year())},{"yLabel",SaveDecimalPlaces(MonthEnteEnergyConsumption.DataTable[0][(Index + 3)],5)} });
			}
			std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
			std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };

			std::vector<Json> Tables;
			for (int Index = 0; Index < 4; Index++)
			{
				std::vector<Json> TableDatas;
				Json TableData;
				TableData["categoryName"] = "";
				TableData["start_color"] = "rgba(65,141,255,1)";
				TableData["end_color"] = "rgba(0,254,233,1)";
				TableData["high_color"] = "#5ceaff";
				TableData["gradient_color"] = "#8FC31F";
				TableData["categoryData"] = CategoryDatas[Index];
				TableDatas.push_back(TableData);

				Json Table;
				Table["title"] = Title[Index];
				Table["unit"] = Unit[Index];
				Table["xLabelRang"] = YearXLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);
			}

			Json ChartData;
			ChartData["title"] = TCHAR_TO_UTF8("年");
			ChartData["type"] = "line";
			ChartData["table"] = Tables;

			ChartDatas.push_back(ChartData);
		}

		//Month
		{
			std::string CurrentEnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption('" + ElecMetID + "','" + StartTime + "','" + EndTime + "')";
			FMysqlResult CurrentEnteEnergyConsumption = MaraidbConnector.Query(CurrentEnteEnergyConsumptionCommand);

			std::string PreYearEnteEnergyConsumptionCommand = "CALL GetEnteEnergyConsumption('" + ElecMetID + "','" + to_iso_extended_string(D_PreYearStartTime) + "','" + to_iso_extended_string(D_PreYearEndTime) + "')";
			FMysqlResult PreYearEnteEnergyConsumption = MaraidbConnector.Query(PreYearEnteEnergyConsumptionCommand);

			FMysqlResult EnteEnergyConsumption[2] = { CurrentEnteEnergyConsumption ,PreYearEnteEnergyConsumption };
			{
				std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
				std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };
				std::string CategoryName[2] = { TCHAR_TO_UTF8("本年度能耗"),TCHAR_TO_UTF8("去年同期能耗") };
				std::string StartColor[2] = { "rgba(133,222,255,0)","rgba(255,255,255,0)" };
				std::string EndColor[2] = { "rgba(133,222,255,1)","rgba(255,255,255,1)" };
				std::string HighColor[2] = { "#5ceaff","#5ceaff" };
				std::string GradientColor[2] = { "#8FC31F","#8FC31F" };

				std::vector<Json> Tables;

				for (int Index = 0; Index < 4; Index++)
				{
					std::vector<Json> TableDatas;

					for (int CategoryIndex = 0; CategoryIndex < 2; CategoryIndex++)
					{
						std::vector<Json>CategoryDatas;
						for (auto EnteEnergyConsumptionRow : EnteEnergyConsumption[CategoryIndex].DataTable)
							CategoryDatas.push_back({
													 {"xLabel",ConvertDateToMonthOnly(EnteEnergyConsumptionRow[0])},
													 {"yLabel",SaveDecimalPlaces(EnteEnergyConsumptionRow[Index == 0 ? 1 : 4 + Index],5)}
								});

						Json TableData;
						TableData["categoryName"] = CategoryName[CategoryIndex];
						TableData["start_color"] = StartColor[CategoryIndex];
						TableData["end_color"] = EndColor[CategoryIndex];
						TableData["high_color"] = HighColor[CategoryIndex];
						TableData["gradient_color"] = GradientColor[CategoryIndex];
						TableData["categoryData"] = FillCategoryData(MonthOnlyXLabelRangs, CategoryDatas);
						TableDatas.push_back(TableData);
					}
					Json Table;
					Table["title"] = Title[Index];
					Table["unit"] = Unit[Index];
					Table["xLabelRang"] = MonthOnlyXLabelRangs;
					Table["data"] = TableDatas;

					Tables.push_back(Table);
				}
				Json ChartData;
				ChartData["title"] = TCHAR_TO_UTF8("月");
				ChartData["type"] = "bar";
				ChartData["table"] = Tables;

				ChartDatas.push_back(ChartData);
			}
		}

		Json Data;
		Data["chartName"] = TCHAR_TO_UTF8("能耗趋势");
		Data["chartData"] = ChartDatas;
		Datas.push_back(Data);
	}

	{
		FMysqlResult EnteOutputValue = MaraidbConnector.Query(str(boost::format("SELECT CreateDate, OutputValue FROM EnteOutputValue WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
		FMysqlResult EnteConsumUnitArea = MaraidbConnector.Query(str(boost::format("SELECT CreateDate, AreaTce FROM EnteEnergyConsumUnitArea WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));
		FMysqlResult EnteEnergyEcnoValueAdd = MaraidbConnector.Query(str(boost::format("SELECT CreateDate, EcnoValueAdd FROM EnteEnergyEcnoValueAdd WHERE ElecMetID ='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % ElecMetID % StartTime % EndTime));

		{
			std::string Title[3] = { TCHAR_TO_UTF8("单位建筑面积能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("度电经济增加值") };
			std::string Unit[3] = { TCHAR_TO_UTF8("tce/m²"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("万元/kW·h") };
			FMysqlResult Result[3] = { EnteConsumUnitArea,EnteOutputValue,EnteEnergyEcnoValueAdd };

			for (int Index = 0; Index < 3; Index++)
			{
				std::vector<Json>ChartDatas;

				std::vector<Json>CategoryDatas;
				for (auto ResultRow : Result[Index].DataTable)
					CategoryDatas.push_back({
											 {"xLabel",ConvertDateToMonth(ResultRow[0],'-')},
											 {"yLabel",SaveDecimalPlaces(ResultRow[1],5)}
						});

				std::vector<Json> TableDatas;
				Json TableData;
				TableData["categoryName"] = "";
				TableData["start_color"] = "rgba(65,141,255,0)";
				TableData["end_color"] = "rgba(65,141,255,1)";
				TableData["high_color"] = "#5ceaff";
				TableData["gradient_color"] = "#8FC31F";
				TableData["categoryData"] = FillCategoryData(MonthXLabelRangs, CategoryDatas);
				TableDatas.push_back(TableData);

				std::vector<Json> Tables;
				Json Table;
				Table["title"] = "";
				Table["unit"] = Unit[Index];
				Table["xLabelRang"] = MonthXLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);

				Json ChartData;
				ChartData["title"] = "";
				ChartData["type"] = "dataZoom";
				ChartData["table"] = Tables;

				ChartDatas.push_back(ChartData);

				Json Data;
				Data["chartName"] = Title[Index];
				Data["chartData"] = ChartDatas;
				Datas.push_back(Data);
			}
		}

	}
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::RegionEnergyEfficiencyAnalysis(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["ID"]);
	std::string YearStartTime = FIELDIFNULL(InData["YearStartTime"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	date D_StartTime = from_string(StartTime);
	date D_EndTime = from_string(EndTime);

	date D_PreYearStartTime = D_StartTime - years(1);
	date D_PreYearEndTime = D_EndTime - years(1);

	FXLabelRangCofig MonthConfig = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);

	std::vector<Json> MonthXLabelRangs = GetXLabelRang(StartTime, EndTime, MonthConfig);

	FXLabelRangCofig MonthOnlyConfig = FXLabelRangCofig(1, false, true, false, '-', EXLabelStepDateIterator::Month);

	std::vector<Json> MonthOnlyXLabelRangs = GetXLabelRang(StartTime, EndTime, MonthOnlyConfig);

	std::vector<Json> Datas;

	{
		std::vector<Json> ChartDatas;

		//Year
		{
			date D_YearStartTime = from_string(YearStartTime);
			std::vector<Json> CategoryDatas[4];
			std::vector<Json> YearXLabelRangs;
			while (D_YearStartTime < D_EndTime)
			{
				date SelectStartDate = D_YearStartTime;
				D_YearStartTime = D_YearStartTime + years(1);
				FMysqlResult MonthIndustryEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT SUM(SynEnergyConsum),SUM(ElecConsumSelf),SUM(WaterConsumSelf),SUM(GasConsumSelf) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % to_iso_extended_string(SelectStartDate) % to_iso_extended_string(D_YearStartTime - days(1))));

				Json XLabelRang;
				XLabelRang["label"] = std::to_string(SelectStartDate.year());
				YearXLabelRangs.push_back(XLabelRang);

				for (int Index = 0; Index < 4; Index++)
					CategoryDatas[Index].push_back({ {"xLabel",std::to_string(SelectStartDate.year())},{"yLabel",SaveDecimalPlaces(MonthIndustryEnergyConsumption.DataTable[0][Index],5)} });
			}
			std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
			std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };

			std::vector<Json> Tables;
			for (int Index = 0; Index < 4; Index++)
			{
				std::vector<Json> TableDatas;
				Json TableData;
				TableData["categoryName"] = "";
				TableData["start_color"] = "rgba(65,141,255,1)";
				TableData["end_color"] = "rgba(0,254,233,1)";
				TableData["high_color"] = "#5ceaff";
				TableData["gradient_color"] = "#8FC31F";
				TableData["categoryData"] = CategoryDatas[Index];
				TableDatas.push_back(TableData);

				Json Table;
				Table["title"] = Title[Index];
				Table["unit"] = Unit[Index];
				Table["xLabelRang"] = YearXLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);
			}

			Json ChartData;
			ChartData["title"] = TCHAR_TO_UTF8("年");
			ChartData["type"] = "line";
			ChartData["table"] = Tables;

			ChartDatas.push_back(ChartData);
		}

		//Month
		{
			FMysqlResult CurrIndustrynteEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,SUM(SynEnergyConsum),SUM(ElecConsumSelf),SUM(WaterConsumSelf),SUM(GasConsumSelf) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY CreateDate ORDER BY CreateDate;") % RegionID % StartTime % EndTime));
			FMysqlResult PreYearIndustryEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,SUM(SynEnergyConsum),SUM(ElecConsumSelf),SUM(WaterConsumSelf),SUM(GasConsumSelf) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY CreateDate ORDER BY CreateDate;") % RegionID % to_iso_extended_string(D_PreYearStartTime) % to_iso_extended_string(D_PreYearEndTime)));

			FMysqlResult IndustryEnergyConsumption[2] = { CurrIndustrynteEnergyConsumption ,PreYearIndustryEnergyConsumption };
			{
				std::string Title[4] = { TCHAR_TO_UTF8("总"),TCHAR_TO_UTF8("电"),TCHAR_TO_UTF8("水"),TCHAR_TO_UTF8("气") };
				std::string Unit[4] = { TCHAR_TO_UTF8("tce"),TCHAR_TO_UTF8("kW·h"),TCHAR_TO_UTF8("m³"),TCHAR_TO_UTF8("m³") };
				std::string CategoryName[2] = { TCHAR_TO_UTF8("本年度能耗"),TCHAR_TO_UTF8("去年同期能耗") };
				std::string StartColor[2] = { "rgba(133,222,255,0)","rgba(255,255,255,0)" };
				std::string EndColor[2] = { "rgba(133,222,255,1)","rgba(255,255,255,1)" };
				std::string HighColor[2] = { "#5ceaff","#5ceaff" };
				std::string GradientColor[2] = { "#8FC31F","#8FC31F" };

				std::vector<Json> Tables;

				for (int Index = 0; Index < 4; Index++)
				{
					std::vector<Json> TableDatas;

					for (int CategoryIndex = 0; CategoryIndex < 2; CategoryIndex++)
					{
						std::vector<Json>CategoryDatas;
						for (auto IndustryEnergyConsumptionRow : IndustryEnergyConsumption[CategoryIndex].DataTable)
							CategoryDatas.push_back({
													 {"xLabel",ConvertDateToMonthOnly(IndustryEnergyConsumptionRow[0])},
													 {"yLabel",SaveDecimalPlaces(IndustryEnergyConsumptionRow[1 + Index],5)}
								});

						Json TableData;
						TableData["categoryName"] = CategoryName[CategoryIndex];
						TableData["start_color"] = StartColor[CategoryIndex];
						TableData["end_color"] = EndColor[CategoryIndex];
						TableData["high_color"] = HighColor[CategoryIndex];
						TableData["gradient_color"] = GradientColor[CategoryIndex];
						TableData["categoryData"] = FillCategoryData(MonthOnlyXLabelRangs, CategoryDatas);
						TableDatas.push_back(TableData);
					}
					Json Table;
					Table["title"] = Title[Index];
					Table["unit"] = Unit[Index];
					Table["xLabelRang"] = MonthOnlyXLabelRangs;
					Table["data"] = TableDatas;

					Tables.push_back(Table);
				}
				Json ChartData;
				ChartData["title"] = TCHAR_TO_UTF8("月");
				ChartData["type"] = "bar";
				ChartData["table"] = Tables;

				ChartDatas.push_back(ChartData);
			}
		}

		Json Data;
		Data["chartName"] = TCHAR_TO_UTF8("能耗趋势");
		Data["chartData"] = ChartDatas;
		Datas.push_back(Data);
	}

	{
		FMysqlResult IndustryEnergyEfficiency = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,SUM(EnergyConsumUnitArea),SUM(OutputValue),AVG(EnergyEcnoValueAdd) FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%' GROUP BY CreateDate ORDER BY CreateDate;") % RegionID % StartTime % EndTime));

		{
			std::string Title[3] = { TCHAR_TO_UTF8("单位建筑面积能耗"),TCHAR_TO_UTF8("万元产值能耗"),TCHAR_TO_UTF8("度电经济增加值") };
			std::string Unit[3] = { TCHAR_TO_UTF8("tce/km²"),TCHAR_TO_UTF8("tce/万元"),TCHAR_TO_UTF8("万元/kW·h") };
			for (int Index = 0; Index < 3; Index++)
			{
				std::vector<Json>ChartDatas;

				std::vector<Json>CategoryDatas;
				for (auto ResultRow : IndustryEnergyEfficiency.DataTable)
					CategoryDatas.push_back({
											 {"xLabel",ConvertDateToMonth(ResultRow[0],'-')},
											 {"yLabel",SaveDecimalPlaces(ResultRow[1 + Index],5)}
						});

				std::vector<Json> TableDatas;
				Json TableData;
				TableData["categoryName"] = "";
				TableData["start_color"] = "rgba(65,141,255,0)";
				TableData["end_color"] = "rgba(65,141,255,1)";
				TableData["high_color"] = "#5ceaff";
				TableData["gradient_color"] = "#8FC31F";
				TableData["categoryData"] = FillCategoryData(MonthXLabelRangs, CategoryDatas);
				TableDatas.push_back(TableData);

				std::vector<Json> Tables;
				Json Table;
				Table["title"] = "";
				Table["unit"] = Unit[Index];
				Table["xLabelRang"] = MonthXLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);

				Json ChartData;
				ChartData["title"] = "";
				ChartData["type"] = "dataZoom";
				ChartData["table"] = Tables;

				ChartDatas.push_back(ChartData);

				Json Data;
				Data["chartName"] = Title[Index];
				Data["chartData"] = ChartDatas;
				Datas.push_back(Data);
			}
		}

	}
	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::RegionMonitoring(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult RegionArea = MaraidbConnector.Query(str(boost::format("SELECT SUM(Area) FROM BaseEnteInfo WHERE FIND_IN_SET(RegionID,GetChildrenRegion('%1%'));") % RegionID));
	double Area = lexical_cast<double>(RegionArea.DataTable[0][0]) / 1000000;

	FMysqlResult RegionCarbonEmission = MaraidbConnector.Query(str(boost::format("SELECT SUM(CarbonEmission) FROM RegionEnergyEfficiencyMonth WHERE FIND_IN_SET(RegionID,GetChildrenRegion('%1%')) AND RegionID !='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % StartTime % EndTime));

	FMysqlResult MonthIndustryEnergyConsumption = MaraidbConnector.Query(str(boost::format("SELECT SynEnergyConsum,ClearEnergyRation FROM RegionEnergyEfficiencyMonth WHERE RegionID='%1%' AND CreateDate BETWEEN '%2%' AND '%3%';") % RegionID % StartTime % EndTime));

	std::vector<Json> Data;

	if (MonthIndustryEnergyConsumption.ResultIsNotNull())
	{

		double SynEnergyConsum = lexical_cast<double>(MonthIndustryEnergyConsumption.DataTable[0][0]);
		double EnergyConsumUnitArea = SynEnergyConsum / Area;

		Data.push_back({ {"name",TCHAR_TO_UTF8("上月总能耗")},{"value",ConvertToTenThousand(MonthIndustryEnergyConsumption.DataTable[0][0]) + TCHAR_TO_UTF8("万tce")} });
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月单位建筑面积能耗")},{"value",ConvertToTenThousand(std::to_string(EnergyConsumUnitArea)) + TCHAR_TO_UTF8("万tce/km²")} });
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月清洁能源占比")},{"value",SaveDecimalPlaces(MonthIndustryEnergyConsumption.DataTable[0][1],2) + "%"} });
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月碳排放")},{"value",ConvertToTenThousand(RegionCarbonEmission.DataTable[0][0]) + TCHAR_TO_UTF8("t·CO₂")} });
	}
	else
	{
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月总能耗")},{"value",TCHAR_TO_UTF8("----万tce")} });
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月单位建筑面积能耗")},{"value",TCHAR_TO_UTF8("----万tce/km²")} });
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月清洁能源占比")},{"value","--%"} });
		Data.push_back({ {"name",TCHAR_TO_UTF8("上月碳排放")},{"value",TCHAR_TO_UTF8("--t·CO₂")} });
	}


	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("全区数据监控");
	ReturnData["data"] = Data;

	return ReturnData;
}
Json OServerHelper::EnteList(Json InData)
{
	using namespace std;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);

	FMysqlResult EnteList = MaraidbConnector.Query(str(boost::format("SELECT EnteName,EnteShortName,IndustryName,RegionName,Address,BaseEnteInfo.ElecMetID FROM BaseEnteInfo,BaseEnteElecMeterInfo LEFT JOIN BaseIndustryInfo ON BaseIndustryInfo.IndustryID=BaseEnteElecMeterInfo.IndustryID LEFT JOIN BaseRegionInfo ON BaseEnteElecMeterInfo.RegionID = BaseRegionInfo.RegionID WHERE BaseEnteInfo.ElecMetID =BaseEnteElecMeterInfo.ElecMetID AND FIND_IN_SET(BaseEnteInfo.RegionID,GetChildrenRegion('%1%'));") % RegionID));

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"企业名称\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"企业简称\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"所属行业\",\"width\":\"25\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"所属区域\",\"width\":\"20\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"用电地址\",\"width\":\"25\"}")));


	std::vector<Json> TableDatas;
	for (auto EnteListRow : EnteList.DataTable)
	{
		std::vector<Json> TableData;
		for (auto EnteListRowValue : EnteListRow)
		{
			Json Value;
			Value["value"] = EnteListRowValue;
			TableData.push_back(Value);
		}
		TableDatas.push_back(TableData);
	}

	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;

	Json Data;
	Data["title"] = "";
	Data["table"] = Table;

	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}
Json OServerHelper::GetHighLineLoadList(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult LineResult = MaraidbConnector.Query(str(boost::format("SELECT ElecLineID,ElecLineName,LoadRate,VoltageLevel,StatElecName FROM LineLoad WHERE LoadRate>50 AND LoadRate<150;")));

	std::vector<Json> Lines;
	for (auto LineResultRow : LineResult.DataTable)
	{
		Json Line;
		Line["ID"] = LineResultRow[0];
		Line["Name"] = LineResultRow[1];
		Line["Load"] = LineResultRow[2];
		Line["VoltageLevel"] = LineResultRow[3];
		Line["PowerStation"] = FillPlaceholderIfNull(LineResultRow[4]);
		Lines.push_back(Line);
	}
	while (Lines < 4)
	{
		Json Line;
		Line["ID"] = "";
		Line["Name"] = "----";
		Line["Load"] = "--";
		Line["VoltageLevel"] = "--";
		Line["PowerStation"] = "-----";
		Lines.push_back(Line);
	}
	Json Data;
	Data["Lines"] = Lines;

	Json ReturnData;
	ReturnData["Data"] = Data;
	return ReturnData;
}
Json OServerHelper::GetExceptionEventInfo(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);

	FMysqlResult EventResult = MaraidbConnector.Query(str(boost::format("SELECT ExceptionEventId,EnteName,FilterDate,AiResult FROM AbnormalEnergyEvent WHERE FilterDate>'%1%' Order By FilterDate DESC LIMIT 10;") % StartTime));

	std::vector<Json> Events;
	for (auto EventResultRow : EventResult.DataTable)
	{
		Json Enent;
		Enent["ID"] = EventResultRow[0];
		Enent["EnteName"] = EventResultRow[1];
		Enent["Date"] = EventResultRow[2];
		Enent["TimeLength"] = "--";
		Enent["AIResult"] = FillPlaceholderIfNull(EventResultRow[3]);
		Events.push_back(Enent);
	}
	while (Events.size() < 4)
	{
		Json Enent;
		Enent["ID"] = "";
		Enent["EnteName"] = "----";
		Enent["Date"] = "------";
		Enent["TimeLength"] = "--";
		Enent["AIResult"] = "--------";
		Events.push_back(Enent);
	}
	Json Data;
	Data["Events"] = Events;

	Json ReturnData;
	ReturnData["Data"] = Data;
	return ReturnData;
}

Json OServerHelper::GetUserMonitor(Json InData)
{
	using namespace cinatra;
	using namespace boost;

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	int UserCount = 0;
	//User
	{
		std::string RequestAPI = HTTP_URL("/largeScrean/user/num");
		auto [ErrorCode, Status, Result] = cinatra::get(RequestAPI);
		if (Status != 200)return{};
		auto HttpReceiveInfo = Json::parse(Result);

		if (!HttpReceiveInfo.is_null())
		{
			for (auto UserDataRow : HttpReceiveInfo["data"])
			{
				UserCount += FIELDIFNULL_FLOAT(UserDataRow["COUNT"]);
			}
		}

	}

	int ViewCount = 0;
	//View
	{
		std::string RequestAPI = HTTP_URL("/largeScrean/login/num");
		std::string RequsetData = "?beginDate=" + StartTime + "&endDate=" + EndTime;
		RequestAPI.append(RequsetData);
		auto [ErrorCode, Status, Result] = cinatra::get(RequestAPI);
		if (Status != 200)return{};
		auto HttpReceiveInfo = Json::parse(Result);

		if (!HttpReceiveInfo.is_null())
		{
			for (auto UserDataRow : HttpReceiveInfo["data"])
			{
				ViewCount += FIELDIFNULL_FLOAT(UserDataRow["num"]);
			}
		}
	}

	Json Register;
	Register["title"] = TCHAR_TO_UTF8("注册用户数量");
	Register["total"] = std::to_string(UserCount);
	Register["unit"] = TCHAR_TO_UTF8("家");

	Json Visit;
	Visit["title"] = TCHAR_TO_UTF8("单日访问次数");
	Visit["total"] = std::to_string(ViewCount);
	Visit["unit"] = TCHAR_TO_UTF8("次");

	Json TableData;
	TableData["register"] = Register;
	TableData["visit"] = Visit;

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("用户分布及单日访问次数统计");
	Data["data"] = TableData;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::UserMonitorDetails(Json InData)
{
	using namespace cinatra;
	using namespace boost;

	std::string PreStartTime = FIELDIFNULL(InData["PreStartTime"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string UserRequestAPI = HTTP_URL("/largeScrean/user/num");
	auto [UserErrorCode, UserStatus, UserResult] = cinatra::get(UserRequestAPI);
	if (UserStatus != 200)return{};
	auto UserHttpReceiveInfo = Json::parse(UserResult);

	std::string VisitRequestAPI = HTTP_URL("/largeScrean/login/num");
	std::string VisitRequsetData = "?beginDate=" + StartTime + "&endDate=" + EndTime;
	VisitRequestAPI.append(VisitRequsetData);
	auto [VisitErrorCode, VisitStatus, VisitResult] = cinatra::get(VisitRequestAPI);
	if (VisitStatus != 200)
		return {};
	auto VisitHttpReceiveInfo = Json::parse(VisitResult);

	std::string PerVisitRequestAPI = HTTP_URL("/largeScrean/login/num");
	std::string PreVisitRequsetData = "?beginDate=" + PreStartTime + "&endDate=" + StartTime;
	PerVisitRequestAPI.append(PreVisitRequsetData);
	auto [PreVisitErrorCode, PreVisitStatus, PreVisitResult] = cinatra::get(PerVisitRequestAPI);
	if (PreVisitStatus != 200)
		return {};
	auto PreVisitHttpReceiveInfo = Json::parse(PreVisitResult);

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"客户类型\",\"width\":\"33.33\"}")));
	RowHeader.push_back(Json::parse((TCHAR_TO_UTF8("{\"headerName\":\"客户数量\",\"width\":\"33.33\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"单日访问量\",\"width\":\"33.33\"}")));

	std::string UserType[4] = { "Customer","Provider","Operate","Government" };
	std::string UserTypeName[4] = { TCHAR_TO_UTF8("能源用户"),TCHAR_TO_UTF8("能源服务商"),TCHAR_TO_UTF8("平台运营"),TCHAR_TO_UTF8("政府用户") };

	std::vector<Json>TableDatas;

	for (int Index=0;Index<4;Index++)
	{
		std::vector<Json>TableData;
		TableData.push_back({ {"value",UserTypeName[Index]} });

		bool bFindUserData = false;
		for (auto DataRow: UserHttpReceiveInfo["data"])
		{
			if (DataRow["TYPE"]== UserType[Index])
			{
				bFindUserData = true;
				TableData.push_back({ {"value",FIELDIFNULL_INT(DataRow["COUNT"])} });
				break;
			}
		}
		if (!bFindUserData)
			TableData.push_back({ {"value",0} });

		bool bFindVisitData = false;
		for (auto DataRow : VisitHttpReceiveInfo["data"])
		{
			if (DataRow["roleCode"] == UserType[Index])
			{
				bFindVisitData = true;
				TableData.push_back({ {"value",FIELDIFNULL_INT(DataRow["num"])} });
			}
		}
		if (!bFindVisitData)
			TableData.push_back({ {"value",0} });

		bool bFindPreVisitData = false;
		for (auto DataRow : PreVisitHttpReceiveInfo["data"])
		{
			if (DataRow["roleCode"] == UserType[Index])
			{
				bFindPreVisitData = true;
				TableData.push_back({ {"value",FIELDIFNULL_INT(DataRow["num"])} });
			}
		}
		if (!bFindPreVisitData)
			TableData.push_back({ {"value",0} });

		TableDatas.push_back(TableData);
	}
	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;

	Json Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("用户分布及单日访问次数");
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::AttentionAnalysis(Json InData)
{
	using namespace cinatra;
	using namespace boost;

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	std::string RequestAPI = HTTP_URL("/largeScrean/click/num");
	std::string RequsetData = "?beginDate=" + StartTime + "&endDate=" + EndTime;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::get(RequestAPI);
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> CategoryDatas;

	if (!HttpReceiveInfo.is_null())
	{
		for (auto DataRow: HttpReceiveInfo["data"])
		{
			Json CategoryData;
			CategoryData["xLabel"] = FIELDIFNULL(DataRow["MODULE_NAME"]);
			CategoryData["yLabel"] = FIELDIFNULL_INT(DataRow["CLICK_NUM"]);
			CategoryDatas.push_back(CategoryData);
		}
	}

	Json TableDatas;
	TableDatas.push_back({ {"categoryData",CategoryDatas} });

	Json Table;
	Table["title"] = TCHAR_TO_UTF8("服务");
	Table["unit"] = TCHAR_TO_UTF8("次");
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("用户关注度分析");
	ReturnData["table"] = Tables;

	return ReturnData;
}

Json OServerHelper::AgentOperation(Json InData)
{
	using namespace cinatra;
	using namespace boost;

	std::string RequestAPI = HTTP_URL("/largeScrean/dyw/num");
	auto [ErrorCode, Status, Result] = cinatra::get(RequestAPI);
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Datas;

	if (!HttpReceiveInfo.is_null())
	{
		std::string KeyNames[3] = { TCHAR_TO_UTF8("代运维公司"),TCHAR_TO_UTF8("代运维客户"),TCHAR_TO_UTF8("代运维总容量") };
		std::string Keys[3] = { "enterpriseNum","electricNum","level" };
		std::string Units[3] = { TCHAR_TO_UTF8("家"),TCHAR_TO_UTF8("家"),"kW" };


		for (int Index=0;Index<3;Index++)
		{
			Json Data;
			Data["key"] = KeyNames[Index];
			Data["value"] = HttpReceiveInfo["data"][Keys[Index]];
			Data["unit"] = Units[Index];
			Datas.push_back(Data);
		}
	}
	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("能源管家业务监控");
	ReturnData["data"] = Datas;

	return ReturnData;

}

Json OServerHelper::GetDealInfo(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult DemandContractInfo = MaraidbConnector.Query("SELECT SUM(ContractAmount),COUNT(ContractId) FROM DemandContractInfo WHERE CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';");
	FMysqlResult GoodsContractInfo = MaraidbConnector.Query("SELECT SUM(ContractAmount),COUNT(ContractId) FROM GoodsContractInfo WHERE CreateDate BETWEEN '" + StartTime + "' AND '" + EndTime + "';");

	std::string DealMoney = ConvertToTenThousand(std::to_string(lexical_cast<double>(ConvertEmptyStringToZero(DemandContractInfo.DataTable[0][0])) + lexical_cast<double>(ConvertEmptyStringToZero(GoodsContractInfo.DataTable[0][0]))));
	std::string DealCount = std::to_string((int)(lexical_cast<double>(DemandContractInfo.DataTable[0][1]) + lexical_cast<double>(GoodsContractInfo.DataTable[0][1])));

	Json Data;
	Data["DealMoney"] = DealMoney;
	Data["DealCount"] = DealCount;

	Json ReturnData;
	ReturnData["Data"] = Data;

	return ReturnData;

}
Json OServerHelper::GetDemandAndGoodsInfo(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult DemandResult = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DemandID) FROM DemandInfo WHERE FlowStatus='审核中';")));
	FMysqlResult GoodsResult = MaraidbConnector.Query(str(boost::format("SELECT COUNT(GoodsID) FROM GoodsInfo WHERE GoodsStatus='审批中';")));

	Json Data;
	Data["DemandCount"] = DemandResult.DataTable[0][0];
	Data["GoodsCount"] = GoodsResult.DataTable[0][0];

	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;

}
Json OServerHelper::MainPageVppEventList(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult EventList = MaraidbConnector.Query(str(boost::format("SELECT EventCode,EventType,StartDate,EndDate,TotolPlan,TotalComplete,EventStatus FROM VppEvent WHERE StartDate BETWEEN '%1%' AND '%2%' Order By StartDate DESC;") % StartTime % EndTime));

	std::vector<Json> RowHeader;
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"事件编号\",\"width\":\"18.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"事件类型\",\"width\":\"14.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"开始时间\",\"width\":\"13.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"结束时间\",\"width\":\"12.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"计划总量(kW)\",\"width\":\"12.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"完成总量(kW)\",\"width\":\"12.27\"}")));
	RowHeader.push_back(Json::parse(TCHAR_TO_UTF8("{\"headerName\":\"状态\",\"width\":\"12.27\"}")));

	std::vector<Json> TableDatas;
	for (auto EventListRow : EventList.DataTable)
	{
		std::vector<Json> TableData;
		for (auto EventListRowValue : EventListRow)
		{
			Json Value;
			Value["value"] = EventListRowValue;
			TableData.push_back(Value);
		}
		TableDatas.push_back(TableData);
	}
	while (TableDatas.size() < 4)
	{
		std::vector<Json> TableData;
		for (auto RowHeader : RowHeader)
		{
			Json Value;
			std::string FillString = "";
			std::string Width = RowHeader["width"];
			for (int i = 0; i < lexical_cast<double>(Width); i++)
				FillString += "-";
			Value["value"] = FillString;
			TableData.push_back(Value);
		}
		TableDatas.push_back(TableData);
	}

	Json Table;
	Table["rowHeader"] = RowHeader;
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("最新削峰填谷事件");
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}
Json OServerHelper::KeyUserDistribution(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string RegionID = FIELDIFNULL(InData["RegionID"]);
	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);
	std::vector<Json> Tables;

	{
		std::string RequestAPI = HTTP_URL("/baseInfo/high/worth/user");
		std::string RequsetData = "?startYear=" + StartTime + "&endYear=" + EndTime + "&regionId=" + RegionID;
		RequestAPI.append(RequsetData);
		auto [ErrorCode, Status, Result] = post(RequestAPI, "");
		if (Status != 200) return{};
		auto HttpReceiveInfo = Json::parse(Result);

		std::vector<Json> Datas;
		Json Data;
		for (auto DataRow : HttpReceiveInfo["data"])
		{
			int KeyUserNum = DataRow["keyUsersI"].is_null() ? 0 : DataRow["keyUsersI"].get<int>();
			KeyUserNum += DataRow["keyUsersII"].is_null() ? 0 : DataRow["keyUsersII"].get<int>();
			FMysqlResult RegionName = MaraidbConnector.Query(str(boost::format("SELECT RegionName FROM BaseRegionInfo WHERE RegionID='%1%';") % (DataRow["regionId"].is_null() ? 0 : DataRow["regionId"].get<std::string>())));

			Json Data;
			Data["name"] = RegionName.DataTable[0][0];
			Data["value"] = std::to_string(KeyUserNum);
			Data["yLabel"] = std::to_string(KeyUserNum);

			Datas.push_back(Data);
		}
		std::sort(Datas.begin(), Datas.end(), SortCategoryDatas);
		Json Table;
		Table["title"] = TCHAR_TO_UTF8("区域");
		Table["data"] = Datas;
		Tables.push_back(Table);
	}

	{
		std::string RequestAPI = HTTP_URL("/baseInfo/industry/high/worth/user");
		std::string RequsetData = "?startYear=" + StartTime + "&endYear=" + EndTime + "&regionId=" + RegionID;
		RequestAPI.append(RequsetData);
		auto [ErrorCode, Status, Result] = post(RequestAPI, "");
		if (Status != 200)return{};
		auto HttpReceiveInfo = Json::parse(Result);

		std::vector<Json> Datas;
		Json Data;
		for (auto DataRow : HttpReceiveInfo["data"])
		{
			int KeyUserNum = DataRow["keyUsersI"].is_null() ? 0 : DataRow["keyUsersI"].get<int>();
			KeyUserNum += DataRow["keyUsersII"].is_null() ? 0 : DataRow["keyUsersII"].get<int>();
			FMysqlResult RegionName = MaraidbConnector.Query(str(boost::format("SELECT IndustryName FROM BaseIndustryInfo WHERE IndustryID='%1%';") % (FIELDIFNULL(DataRow["industryGbCode"]))));

			if (RegionName.ResultIsNotNull())
			{
				Json Data;
				Data["name"] = RegionName.DataTable[0][0];
				Data["value"] = std::to_string(KeyUserNum);
				Data["yLabel"] = std::to_string(KeyUserNum);

				Datas.push_back(Data);
			}

		}
		std::sort(Datas.begin(), Datas.end(), SortCategoryDatas);
		Json Table;
		Table["title"] = TCHAR_TO_UTF8("行业");
		Table["data"] = Datas;
		Tables.push_back(Table);
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("关键用户区域行业分布");
	ReturnData["tables"] = Tables;
	return ReturnData;
}

Json OServerHelper::KeyUserAnalyze(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace boost::gregorian;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string RegionID = FIELDIFNULL(InData["RegionID"]);


	date CurrentYear = from_string(FillDate(StartTime));
	date PreYear = CurrentYear - years(1);

	std::vector<Json> RegionContentData[2];
	std::vector<Json> RegionContentData_Ratio[2];

	std::vector<Json> IndustryContentData[2];
	std::vector<Json> IndustryContentData_Ratio[2];

	//Region
	{

		std::string ChildRegionListCommand = "SELECT RegionID,RegionName FROM BaseRegionInfo WHERE BaseRegionInfo.ParentID='"+ RegionID+"';";
		FMysqlResult ChildRegionList = MaraidbConnector.Query(ChildRegionListCommand);
		for (auto ChildRegionRow : ChildRegionList.DataTable)
		{
			FMysqlResult CurrentYearResult = MaraidbConnector.Query(str(boost::format("SELECT ValuePeakPercent,UnitElectricityCharge FROM RegionKeyUserAnalysis WHERE RegionID='%1%' AND DateOfYear='%2%';") % ChildRegionRow[0] % std::to_string(CurrentYear.year())));
			FMysqlResult PreYearResult = MaraidbConnector.Query(str(boost::format("SELECT ValuePeakPercent,UnitElectricityCharge FROM RegionKeyUserAnalysis WHERE RegionID='%1%' AND DateOfYear='%2%';") % ChildRegionRow[0] % std::to_string(PreYear.year())));

			float ValuePeakPercent_CurrentYear = 0;
			float UnitElectricityCharge_CurrentYear = 0;
			float ValuePeakPercent_PreYear = 0;
			float UnitElectricityCharge_PreYear = 0;

			if (CurrentYearResult.ResultIsNotNull() && PreYearResult.ResultIsNotNull())
			{
				ValuePeakPercent_CurrentYear = lexical_cast<float>(CurrentYearResult.DataTable[0][0]);
				UnitElectricityCharge_CurrentYear = lexical_cast<float>(CurrentYearResult.DataTable[0][1]);
				ValuePeakPercent_PreYear = lexical_cast<float>(PreYearResult.DataTable[0][0]);
				UnitElectricityCharge_PreYear = lexical_cast<float>(PreYearResult.DataTable[0][1]);
			}
			RegionContentData[0].push_back({ {"name",ChildRegionRow[1]},{"value",SaveDecimalPlaces(std::to_string(ValuePeakPercent_CurrentYear * 100))} });
			RegionContentData_Ratio[0].push_back({ {"name",ChildRegionRow[1]},{"value",SaveDecimalPlaces(std::to_string(ValuePeakPercent_PreYear == 0 ? 0 : 100 * (ValuePeakPercent_CurrentYear / ValuePeakPercent_PreYear - 1)))} });

			RegionContentData[1].push_back({ {"name",ChildRegionRow[1]},{"value",SaveDecimalPlaces(std::to_string(UnitElectricityCharge_CurrentYear))} });
			RegionContentData_Ratio[1].push_back({ {"name",ChildRegionRow[1]},{"value",SaveDecimalPlaces(std::to_string(UnitElectricityCharge_PreYear == 0 ? 0 : 100 * (UnitElectricityCharge_CurrentYear / UnitElectricityCharge_PreYear - 1)))} });
		}

	}

	//Industry
	{
		std::string ChildIndustryListCommand = "SELECT IndustryID,IndustryName FROM BaseIndustryInfo WHERE ParentID='' OR ISNULL(ParentID);";
		FMysqlResult ChildIndustryList = MaraidbConnector.Query(ChildIndustryListCommand);
		for (auto ChildIndustryRow : ChildIndustryList.DataTable)
		{
			FMysqlResult CurrentYearResult = MaraidbConnector.Query(str(boost::format("SELECT ValuePeakPercent,UnitElectricityCharge FROM IndustryKeyUserAnalysis WHERE RegionID='%1%'  AND IndustryID='%2%'  AND DateOfYear='%3%';") % RegionID % ChildIndustryRow[0] % std::to_string(CurrentYear.year())));
			FMysqlResult PreYearResult = MaraidbConnector.Query(str(boost::format("SELECT ValuePeakPercent,UnitElectricityCharge FROM IndustryKeyUserAnalysis WHERE RegionID='%1%' AND IndustryID='%2%' AND DateOfYear='%3%';") % RegionID % ChildIndustryRow[0] % std::to_string(PreYear.year())));

			float ValuePeakPercent_CurrentYear = 0;
			float UnitElectricityCharge_CurrentYear = 0;
			float ValuePeakPercent_PreYear = 0;
			float UnitElectricityCharge_PreYear = 0;

			if (CurrentYearResult.ResultIsNotNull() && PreYearResult.ResultIsNotNull())
			{
				ValuePeakPercent_CurrentYear = lexical_cast<float>(CurrentYearResult.DataTable[0][0]);
				UnitElectricityCharge_CurrentYear = lexical_cast<float>(CurrentYearResult.DataTable[0][1]);
				ValuePeakPercent_PreYear = lexical_cast<float>(PreYearResult.DataTable[0][0]);
				UnitElectricityCharge_PreYear = lexical_cast<float>(PreYearResult.DataTable[0][1]);
			}
			IndustryContentData[0].push_back({ {"name",ChildIndustryRow[1]},{"value",SaveDecimalPlaces(std::to_string(ValuePeakPercent_CurrentYear * 100))} });
			IndustryContentData_Ratio[0].push_back({ {"name",ChildIndustryRow[1]},{"value",SaveDecimalPlaces(std::to_string(ValuePeakPercent_PreYear == 0 ? 0 : 100 * (ValuePeakPercent_CurrentYear / ValuePeakPercent_PreYear - 1)))} });

			IndustryContentData[1].push_back({ {"name",ChildIndustryRow[1]},{"value",SaveDecimalPlaces(std::to_string(UnitElectricityCharge_CurrentYear))} });
			IndustryContentData_Ratio[1].push_back({ {"name",ChildIndustryRow[1]},{"value",SaveDecimalPlaces(std::to_string(UnitElectricityCharge_PreYear == 0 ? 0 : 100 * (UnitElectricityCharge_CurrentYear / UnitElectricityCharge_PreYear - 1)))} });
		}

	}

	std::string TableName[2] = { TCHAR_TO_UTF8("年均峰时段电量占比"),TCHAR_TO_UTF8("年均度电电费") };
	std::string DataType[2] = { TCHAR_TO_UTF8("区域"),TCHAR_TO_UTF8("行业") };
	std::string Unit[2] = { "%",TCHAR_TO_UTF8("元/kW·h") };

	std::vector<Json> ChartDatas;

	for (int TableIndex = 0; TableIndex < 2; TableIndex++)
	{
		std::vector<Json> TableDatas;

		for (int DataTypeIndex = 0; DataTypeIndex < 2; DataTypeIndex++)
		{
			std::vector<Json> TableDataContents;
			for (int CategoryIndex = 0; CategoryIndex < 2; CategoryIndex++)
			{
				Json TableDataContent;
				TableDataContent["title"] = TableName[TableIndex] + (CategoryIndex == 0 ? " " : TCHAR_TO_UTF8("增长率"));
				TableDataContent["unit"] = Unit[TableIndex];
				TableDataContent["data"] = DataTypeIndex == 0 ? (CategoryIndex == 0 ? RegionContentData[TableIndex] : RegionContentData_Ratio[TableIndex]) : (CategoryIndex == 0 ? IndustryContentData[TableIndex] : IndustryContentData_Ratio[TableIndex]);
				TableDataContents.push_back(TableDataContent);
			}

			Json TableData;
			TableData["dataType"] = DataType[DataTypeIndex];
			TableData["content"] = TableDataContents;
			TableDatas.push_back(TableData);
		}
		Json ChartData;
		ChartData["tableName"] = TableName[TableIndex];
		ChartData["tableData"] = TableDatas;
		ChartDatas.push_back(ChartData);
	}
	Json ReturnData;
	ReturnData["chartName"] = TCHAR_TO_UTF8("高净值用户区域及行业分析");
	ReturnData["chartData"] = ChartDatas;

	return ReturnData;
}

Json OServerHelper::LineLoad(Json InData)
{
	using namespace  boost::posix_time;
	using namespace  boost::gregorian;

	std::string LineID = FIELDIFNULL(InData["LineID"]);
	std::string Time = FIELDIFNULL(InData["Time"]);

	std::vector<Json> XLabelRangs;
	ptime XLabelTime = time_from_string(Time + " 00:00:00");
	for (int Index = 0; Index < 24; Index++)
	{
		std::string CurrentTime = to_simple_string(XLabelTime.time_of_day());
		CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
		Json XLabelRang;
		XLabelRang["label"] = CurrentTime;
		XLabelRangs.push_back(XLabelRang);
		XLabelTime = XLabelTime + hours(1);
	}

	std::string RequestAPI = HTTP_URL("/energyManage/elec/line/load/data");
	std::string RequsetData = "?elecLineId=" + LineID + "&searchDate=" + Time;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> CategoryDatas;
	for (auto Row : HttpReceiveInfo["data"])
	{
		ptime XLabelTime = time_from_string(Row["putDate"]);
		std::string CurrentTime = to_simple_string(XLabelTime.time_of_day());
		CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);

		Json CategoryData;
		CategoryData["xLabel"] = CurrentTime;
		CategoryData["yLabel"] = std::to_string(FIELDIFNULL_FLOAT(Row["valueLoadRate"]));
		CategoryDatas.push_back(CategoryData);
	}

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("实时负载");
	TableData["icon"] = "auto";
	TableData["lineStyle"] = "solid";
	TableData["start_color"] = "rgba(255,255,255)";
	TableData["end_color"] = "rgba(255,255,255)";
	TableData["high_color"] = "rgba(97,173,255,0.3)";
	TableData["gradient_color"] = "rgba(97,173,255,0.05)";
	TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);


	std::vector<Json>TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = "%";
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json>Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("负载曲线");
	Data["table"] = Tables;

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}
Json OServerHelper::PeakCuttingAndValleyFillingEvents(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FMysqlResult PeakClipEventInfo = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT EventCode),SUM(TotalComplete),SUM(TotolMoney) FROM VppEvent WHERE EventType='削峰事件' AND StartDate>='%1%' AND EndDate<='%2%';") % StartTime % EndTime));
	FMysqlResult ValleyFillEventInfo = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT EventCode),SUM(TotalComplete),SUM(TotolMoney) FROM VppEvent WHERE EventType='填谷事件' AND StartDate>='%1%' AND EndDate<='%2%';") % StartTime % EndTime));

	int EventCount = 0;
	double PeakClip = 0;
	double ValleyFill = 0;
	double Money = 0;

	if (PeakClipEventInfo.ResultIsNotNull())
	{
		EventCount += lexical_cast<int>(ConvertEmptyStringToZero(PeakClipEventInfo.DataTable[0][0]));
		PeakClip = lexical_cast<double>(ConvertEmptyStringToZero(PeakClipEventInfo.DataTable[0][1]));
		Money += lexical_cast<double>(ConvertEmptyStringToZero(PeakClipEventInfo.DataTable[0][2]));
	}
	if (ValleyFillEventInfo.ResultIsNotNull())
	{
		EventCount += lexical_cast<int>(ConvertEmptyStringToZero(ValleyFillEventInfo.DataTable[0][0]));
		ValleyFill = lexical_cast<double>(ConvertEmptyStringToZero(ValleyFillEventInfo.DataTable[0][1]));
		Money += lexical_cast<double>(ConvertEmptyStringToZero(ValleyFillEventInfo.DataTable[0][2]));
	}

	std::vector<Json> Tables;
	Tables.push_back({
						  {"title",TCHAR_TO_UTF8("本年执行事件")},
						  {"xLabel",TCHAR_TO_UTF8("次")},
						  {"yLabel",std::to_string(EventCount)}
		});
	Tables.push_back({
						  {"title",TCHAR_TO_UTF8("削峰负荷总量")},
						  {"xLabel","kw"},
						  {"yLabel",SaveDecimalPlaces(std::to_string(PeakClip),2)}
		});
	Tables.push_back({
						  {"title",TCHAR_TO_UTF8("填谷负荷总量")},
						  {"xLabel","kw"},
						  {"yLabel",SaveDecimalPlaces(std::to_string(ValleyFill),2)}
		});
	Tables.push_back({
						  {"title",TCHAR_TO_UTF8("本年总激励金额")},
						  {"xLabel",TCHAR_TO_UTF8("万元")},
						  {"yLabel",ConvertToTenThousand(SaveDecimalPlaces(std::to_string(Money),2))}
		});

	std::vector<Json> Datas;
	Json Data;
	Data["title"] = TCHAR_TO_UTF8("削峰填谷事件概览");
	Data["table"] = Tables;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::PeakCuttingAndValleyFillingEventLineList(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/vpp/event/line");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	int Index = 0;
	string JudgeString = "";
	std::vector<std::vector<std::string>> LineTable;
	for (auto LineID: HttpReceiveInfo["data"])
	{
		JudgeString += "ElecLineID= '";
		JudgeString += FIELDIFNULL(LineID);
		JudgeString += "' OR ";
		Index++;
		if (Index>20)
		{
			FMysqlResult LineInfo = MaraidbConnector.Query("SELECT ElecLineID,ElecLineName,StatElecName FROM LineLoad WHERE "+ JudgeString+ " 0; ");
			LineTable.insert(LineTable.end(), LineInfo.DataTable.begin(), LineInfo.DataTable.end());
		}
	}
	if (JudgeString!="")
	{
		FMysqlResult LineInfo = MaraidbConnector.Query("SELECT ElecLineID,ElecLineName,StatElecName FROM LineLoad WHERE " + JudgeString + " 0; ");
		LineTable.insert(LineTable.end(), LineInfo.DataTable.begin(), LineInfo.DataTable.end());
	}
	std::vector<Json> Datas;
	for (auto LineTableRow: LineTable)
	{
		Json Data;
		Data["LineID"] = LineTableRow[0];
		Data["LineName"] = LineTableRow[1];
		Data["PowerStation"] = LineTableRow[2];
		Datas.push_back(Data);
	}

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;
}

Json OServerHelper::PendingToResponseEnteListInVppEvent(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Datas;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("等待回复"))
		{
			FMysqlResult EnteInfo = MaraidbConnector.Query(str(boost::format("SELECT EnteName,ContactPerson,ContactPhone FROM BaseEnteInfo WHERE ElecMetID='%1%';") % DataRow["elecNum"]));
			if (EnteInfo.ResultIsNotNull())
			{
				Json Data;
				Data[TCHAR_TO_UTF8("企业名称")] = EnteInfo.DataTable[0][0];
				Data[TCHAR_TO_UTF8("联系人")] = EnteInfo.DataTable[0][1];
				Data[TCHAR_TO_UTF8("联系电话")] = EnteInfo.DataTable[0][2];
				Datas.push_back(Data);
			}
		}
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("待应答企业");
	ReturnData["data"] = Datas;

	return ReturnData;

}

Json OServerHelper::RefuseEnteListInVppEvent(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> TableData;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("拒绝邀约"))
		{
			Json Data;
			Data[TCHAR_TO_UTF8("企业名称")] = DataRow["enteElecName"];
			Data[TCHAR_TO_UTF8("拒绝原因")] = FIELDIFNULL(DataRow["feedbackContent"]);
			TableData.push_back(Data);
		}
	}
	Json Table;
	Table["data"] = TableData;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("拒绝邀约企业");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::AgreeEnteListInVppEvent(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Datas;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
		{
			Json Data;
			Data[TCHAR_TO_UTF8("企业名称")] = FIELDIFNULL(DataRow["enteElecName"]);
			Data[TCHAR_TO_UTF8("响应量")] = (FIELDIFNULL(DataRow["respFeedback"])) + "kw";
			Datas.push_back(Data);
		}
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("同意邀约企业");
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnteListInVppEvent(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	std::string SelectLineID = FIELDIFNULL(FIELDIFNULL(InData["LineID"]));


	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	Json Data;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		string LineID = FIELDIFNULL(DataRow["elecLineId"]);
		if (LineID != SelectLineID && SelectLineID != "")continue;
		FMysqlResult LineInfo = MaraidbConnector.Query(str(boost::format("SELECT ElecLineName,StatElecName FROM LineLoad WHERE ElecLineID='%1%';") % LineID));

		std::string LineName = "";
		std::string StationName = "";

		if (LineInfo.ResultIsNotNull())
		{
			LineName = LineInfo.DataTable[0][0];
			StationName = LineInfo.DataTable[0][1];
		}
		float RealResp = FIELDIFNULL_FLOAT(DataRow["realResp"]);
		float PlanResp = lexical_cast<float>(DataRow["respFeedback"].is_null() ?"0": DataRow["respFeedback"].get<std::string>());

		Json EnteInfo;
		EnteInfo["TargetID"] = FIELDIFNULL(DataRow["targetId"]);
		EnteInfo["ElecNum"] = FIELDIFNULL(DataRow["elecNum"]);
		EnteInfo["EnteElecName"] = FIELDIFNULL(DataRow["enteElecName"]);
		EnteInfo["LineName"] = LineName;
		EnteInfo["PowerStation"] = StationName;
		EnteInfo["FeedbackStatus"] = FIELDIFNULL(DataRow["feedbackStatus"]);
		EnteInfo["ConventionalResponseQuantity"] = FIELDIFNULL(DataRow["respFeedback"]);
		EnteInfo["ActualResponseQuantity"] =SaveDecimalPlaces(std::to_string(RealResp));
		EnteInfo["PercentageComplete"] = PlanResp == 0 ? "0" : SaveDecimalPlaces(std::to_string(RealResp / PlanResp));
		EnteInfo["IncentivesAre"] = FIELDIFNULL(DataRow["totalMoney"]);

		if (EnteInfo["FeedbackStatus"] == TCHAR_TO_UTF8("同意邀约"))
		{
			Data["AgreeToInviteBusiness"].push_back(EnteInfo);
		}
		else if (EnteInfo["FeedbackStatus"] == TCHAR_TO_UTF8("拒绝邀约"))
		{
			Data["RefuseToInviteBusiness"].push_back(EnteInfo);
		}
		else
		{
			Data["WaitingForReply"].push_back(EnteInfo);
		}
	}

	Json ReturnData;
	ReturnData["data"] = Data;

	return ReturnData;
}

Json OServerHelper::InviteCountDown(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	FMysqlResult StartTime = MaraidbConnector.Query(str(boost::format("SELECT StartDate FROM VppEvent WHERE EventCode='%1%';") % EventCode));

	if (StartTime.ResultIsNotNull())
	{
		Json ReturnData;
		ReturnData["title"] = TCHAR_TO_UTF8("邀约倒计时");
		ReturnData["dateTime"] = StartTime.DataTable[0][0];
		return ReturnData;
	}
	return {};
}

Json OServerHelper::InviteProgress(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	FMysqlResult TotolPlan = MaraidbConnector.Query(str(boost::format("SELECT TotolPlan FROM VppEvent WHERE EventCode='%1%';") % EventCode));

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	double TotleRespon = 0;

	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
		{
			try
			{
				TotleRespon += lexical_cast<double>((FIELDIFNULL(DataRow["respFeedback"])));
			}
			catch (const std::exception &)
			{

			}
		}
	}

	if (TotolPlan.ResultIsNotNull())
	{
		std::string  Pre = "";
		try
		{
			double Plan = lexical_cast<double>(TotolPlan.DataTable[0][0]);
			Pre = SaveDecimalPlaces(std::to_string(TotleRespon / Plan * 100), 0);
			std::vector<Json> TableValue;
			TableValue.push_back({ {"title",TCHAR_TO_UTF8("计划总量")},{"value",TotolPlan.DataTable[0][0] + "kW"} });
			TableValue.push_back({ {"title",TCHAR_TO_UTF8("响应量")},{"value",SaveDecimalPlaces(std::to_string(TotleRespon)) + "kW"} });

			Json Table;
			Table["title"] = "";
			Table["unit"] = "kW";
			Table["pre"] = Pre;
			Table["tableValue"] = TableValue;

			std::vector<Json> Tables;
			Tables.push_back(Table);

			Json Data;
			Data["title"] = TCHAR_TO_UTF8("邀约进度");
			Data["table"] = Tables;

			std::vector<Json> Datas;
			Datas.push_back(Data);

			Json ReturnData;
			ReturnData["data"] = Datas;

			return ReturnData;
		}
		catch (const std::exception &)
		{
			return {};
		}

	}
	return {};
}

Json OServerHelper::ExecuteCountDown(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	FMysqlResult StartTime = MaraidbConnector.Query(str(boost::format("SELECT EndDate FROM VppEvent WHERE EventCode='%1%';") % EventCode));

	if (StartTime.ResultIsNotNull())
	{
		Json ReturnData;
		ReturnData["title"] = TCHAR_TO_UTF8("执行倒计时");
		ReturnData["dateTime"] = StartTime.DataTable[0][0];
		return ReturnData;
	}
	return {};
}

Json OServerHelper::ExecuteProgress(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	FMysqlResult TotolPlan = MaraidbConnector.Query(str(boost::format("SELECT TotolPlan FROM VppEvent WHERE EventCode='%1%';") % EventCode));

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	struct EventExcuteInfo
	{
		std::string EnteName = "";
		std::string RealResp = "0";
		std::string RespFeedback = "0";
	};
	double TotleRespon = 0;
	EventExcuteInfo TopEnte[3];
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
		{
			try
			{
				double CurrentEnteRealResp = lexical_cast<double>(DataRow["realResp"]);
				TotleRespon += CurrentEnteRealResp;
				for (int i = 0; i < 3; i++)
				{
					double TopEnteRealResp = lexical_cast<double>(TopEnte[i].RealResp);
					if (CurrentEnteRealResp > TopEnteRealResp)
					{
						EventExcuteInfo CurrentEnteInfo;
						CurrentEnteInfo.EnteName = FIELDIFNULL(DataRow["enteElecName"]);
						CurrentEnteInfo.RealResp = std::to_string(FIELDIFNULL_FLOAT(DataRow["realResp"]));
						CurrentEnteInfo.RespFeedback = (FIELDIFNULL(DataRow["respFeedback"]));

						switch (i)
						{
						case 0:
							TopEnte[2] = TopEnte[1];
							TopEnte[1] = TopEnte[0];
							TopEnte[0] = CurrentEnteInfo;
							break;
						case 1:
							TopEnte[2] = TopEnte[1];
							TopEnte[1] = CurrentEnteInfo;
							break;

						case 2:
							TopEnte[2] = CurrentEnteInfo;
							break;

						default:
							break;
						}
						break;
					}
				}

			}
			catch (const std::exception &)
			{

			}
		}
	}

	std::vector<Json> TableValues;
	for (int i = 0; i < 3; i++)
	{
		Json TableValue;

		std::vector<Json> TRS;
		TRS.push_back({ {"td1",TopEnte[i].EnteName},{"td2",TCHAR_TO_UTF8("完成量")} });
		TRS.push_back({ {"td1",TCHAR_TO_UTF8("响应量: ") + SaveDecimalPlaces(TopEnte[i].RealResp) + " kw"},{"td2",TCHAR_TO_UTF8("完成度: ") + SaveDecimalPlaces(std::to_string(lexical_cast<double>(TopEnte[i].RealResp) / lexical_cast<double>(TopEnte[i].RespFeedback) * 100),2) + "%"} });
		TableValue["tr"] = TRS;
		TableValues.push_back(TableValue);
	}

	if (TotolPlan.ResultIsNotNull())
	{
		std::string  Pre = "";
		try
		{
			double Plan = lexical_cast<double>(TotolPlan.DataTable[0][0]);
			Pre = SaveDecimalPlaces(std::to_string(TotleRespon / Plan * 100), 0);


			Json Table;
			Table["title"] = "";
			Table["precent"] = Pre;
			Table["unit"] = "";
			Table["tableValue"] = TableValues;

			std::vector<Json> Tables;
			Tables.push_back(Table);

			Json Data;
			Data["title"] = TCHAR_TO_UTF8("执行进度");
			Data["table"] = Tables;

			std::vector<Json> Datas;
			Datas.push_back(Data);

			Json ReturnData;
			ReturnData["data"] = Datas;

			return ReturnData;
		}
		catch (const std::exception &)
		{
			return {};
		}

	}
	return {};

}

Json OServerHelper::TimeLine(Json InData)
{
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);
	FMysqlResult EventStatus = MaraidbConnector.Query(str(boost::format("SELECT EventStatus FROM VppEvent WHERE EventCode='%1%';") % EventCode));
	std::vector<Json> Datas;

	if (!EventStatus.ResultIsNotNull())
	{

		Datas.push_back({ {"name",TCHAR_TO_UTF8("冻结负荷数据")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("计算奖励金额")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("公式阶段")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("结算并执行")},{"state",TCHAR_TO_UTF8("未完成")} });
		Json ReturnData;
		ReturnData["data"] = Datas;

		return ReturnData;
	}

	if (EventStatus.DataTable[0][0] == TCHAR_TO_UTF8("完结"))
	{
		Datas.push_back({ {"name",TCHAR_TO_UTF8("冻结负荷数据")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("计算奖励金额")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("公式阶段")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("结算并执行")},{"state",TCHAR_TO_UTF8("完成")} });
	}
	else if (EventStatus.DataTable[0][0] == TCHAR_TO_UTF8("结算公示"))
	{
		Datas.push_back({ {"name",TCHAR_TO_UTF8("冻结负荷数据")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("计算奖励金额")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("公式阶段")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("结算并执行")},{"state",TCHAR_TO_UTF8("未完成")} });
	}
	else if (EventStatus.DataTable[0][0] == TCHAR_TO_UTF8("结算中(等待计算)"))
	{
		Datas.push_back({ {"name",TCHAR_TO_UTF8("冻结负荷数据")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("计算奖励金额")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("公式阶段")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("结算并执行")},{"state",TCHAR_TO_UTF8("未完成")} });
	}
	else if (EventStatus.DataTable[0][0] == TCHAR_TO_UTF8("结算中(待冻结)"))
	{
		Datas.push_back({ {"name",TCHAR_TO_UTF8("冻结负荷数据")},{"state",TCHAR_TO_UTF8("完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("计算奖励金额")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("公式阶段")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("结算并执行")},{"state",TCHAR_TO_UTF8("未完成")} });
	}
	else
	{
		Datas.push_back({ {"name",TCHAR_TO_UTF8("冻结负荷数据")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("计算奖励金额")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("公式阶段")},{"state",TCHAR_TO_UTF8("未完成")} });
		Datas.push_back({ {"name",TCHAR_TO_UTF8("结算并执行")},{"state",TCHAR_TO_UTF8("未完成")} });
	}
	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("结算进度");
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnteContributeCurveByTargetID(Json InData)
{
	using namespace boost;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string TargetID = FIELDIFNULL(InData["TargetID"]);

	std::vector<Json> Tables;
	Json Data;

	FMysqlResult RealCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoad WHERE TargetID='%1%' ORDER BY CreateDate;") % TargetID));
	FMysqlResult BaseCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoadBase WHERE TargetID='%1%' ORDER BY CreateDate;") % TargetID));

	for (auto RealCurveResultRow : RealCurveResult.DataTable)
	{
		Json RealCurveInfo;
		RealCurveInfo["pointTime"] = RealCurveResultRow[0];
		RealCurveInfo["pointValue"] = RealCurveResultRow[1];
		Data["vppEventLoadList"].push_back(RealCurveInfo);
	}
	for (auto BaseCurveResultRow : BaseCurveResult.DataTable)
	{
		Json BaseCurveInfo;
		BaseCurveInfo["pointTime"] = BaseCurveResultRow[0];
		BaseCurveInfo["pointValue"] = BaseCurveResultRow[1];
		Data["vppEventBaseList"].push_back(BaseCurveInfo);
	}

	std::vector<Json>Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EnteContributeCurve(Json InData)
{
	using namespace boost;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Datas;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		std::string TargetID = FIELDIFNULL(DataRow["targetId"]);
		std::string LineID = FIELDIFNULL(DataRow["elecLineId"]);

		FMysqlResult RealCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoad WHERE TargetID='%1%' ORDER BY CreateDate;") % TargetID));
		FMysqlResult BaseCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoadBase WHERE TargetID='%1%' ORDER BY CreateDate;") % TargetID));
		FMysqlResult LineInfo = MaraidbConnector.Query(str(boost::format("SELECT ElecLineID,ElecLineName,StatElecName FROM LineLoad WHERE ElecLineID='%1%'")% LineID));

		float RealResp = FIELDIFNULL_FLOAT(DataRow["realResp"]);
		float PlanResp = lexical_cast<float>(DataRow["respFeedback"].is_null() ? "0" : DataRow["respFeedback"].get<std::string>());

		Json Data;
		Data["title"] = FIELDIFNULL(DataRow["enteElecName"]);
		Data["line"] = LineInfo.ResultIsNotNull()? LineInfo.DataTable[0][1]:"----";
		Data["agreementQuantity"] = FIELDIFNULL(DataRow["respFeedback"]);
		Data["finishQuantity"] = SaveDecimalPlaces(std::to_string(RealResp));
		Data["finishDeg"] = PlanResp == 0 ? "0" : SaveDecimalPlaces(std::to_string(RealResp / PlanResp));

		for (auto RealCurveResultRow : RealCurveResult.DataTable)
		{
			Json RealCurveInfo;
			RealCurveInfo["xLabel"] = RealCurveResultRow[0];
			RealCurveInfo["yLabel"] = RealCurveResultRow[1];
			Data["BaseLine"].push_back(RealCurveInfo);
		}
		for (auto BaseCurveResultRow : BaseCurveResult.DataTable)
		{
			Json BaseCurveInfo;
			BaseCurveInfo["xLabel"] = BaseCurveResultRow[0];
			BaseCurveInfo["yLabel"] = BaseCurveResultRow[1];
			Data["ActualBaseLine"].push_back(BaseCurveInfo);
		}
		Datas.push_back(Data);
	}

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;

}

Json OServerHelper::EnteContributeCurve_Execute(Json InData)
{
	using namespace boost;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	std::vector<Json> Tables;

	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
		{
			try
			{


				FMysqlResult RealCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoad WHERE TargetID='%1%' ORDER BY CreateDate;") % (FIELDIFNULL(DataRow["targetId"]))));
				FMysqlResult BaseCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoadBase WHERE TargetID='%1%' ORDER BY CreateDate;") % (FIELDIFNULL(DataRow["targetId"]))));

				std::vector<Json> XLabelRangs;
				if (BaseCurveResult.ResultIsNotNull())
				{
					for (auto CurveRow : BaseCurveResult.DataTable)
					{
						Json XLabelRang;
						ptime Time = time_from_string(CurveRow[0]);
						std::string CurrentTime = to_simple_string(Time.time_of_day());
						CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
						XLabelRang["label"] = CurrentTime;
						XLabelRangs.push_back(XLabelRang);
					}
				}
				else
				{
					ptime XLabelTime = time_from_string("1970-01-01 00:00:00");
					for (int Index = 0; Index < 24; Index++)
					{
						std::string CurrentTime = to_simple_string(XLabelTime.time_of_day());
						CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
						Json XLabelRang;
						XLabelRang["label"] = CurrentTime;
						XLabelRangs.push_back(XLabelRang);
						XLabelTime = XLabelTime + hours(1);
					}
				}

				FMysqlResult Curve[2] = { RealCurveResult ,BaseCurveResult };
				std::string CategoryName[2] = { TCHAR_TO_UTF8("实际负荷"),TCHAR_TO_UTF8("基线") };
				std::string StartColor[2] = { "rgba(255,253,255,1)","rgba(0,253,173,1)" };
				std::string HighColor[2] = { "rgba(255,253,255,0.3)","rgba(97,173,255,0.3)" };
				std::string GradientColor[2] = { "rgba(0,253,173,0)","rgba(97,173,255,0)" };

				std::vector<Json> TableDatas;
				for (int CategoryIndex = 0; CategoryIndex < 2; CategoryIndex++)
				{
					std::vector<Json> CategoryDatas;
					for (auto CurveRow : Curve[CategoryIndex].DataTable)
					{
						ptime Time = time_from_string(CurveRow[0]);
						std::string CurrentTime = to_simple_string(Time.time_of_day());
						CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
						CategoryDatas.push_back({ {"xLabel",CurrentTime},{"yLabel",CurveRow[1]} });
					}

					Json TableData;
					TableData["categoryName"] = CategoryName[CategoryIndex];
					TableData["icon"] = "auto";
					TableData["lineStyle"] = "solid";
					TableData["start_color"] = StartColor[CategoryIndex];
					TableData["end_color"] = StartColor[CategoryIndex];
					TableData["high_color"] = HighColor[CategoryIndex];
					TableData["gradient_color"] = GradientColor[CategoryIndex];
					TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
					TableDatas.push_back(TableData);
				}

				Json Table;
				Table["title"] = FIELDIFNULL(DataRow["enteElecName"]);
				Table["unit"] = "kW";
				Table["xLabelRang"] = XLabelRangs;
				Table["data"] = TableDatas;

				Tables.push_back(Table);



			}
			catch (const std::exception &)
			{

			}
		}
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("企业出力曲线");
	ReturnData["table"] = Tables;

	return ReturnData;
}

Json OServerHelper::EnteContributeCurve_Finish(Json InData)
{
	using namespace boost;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string EventCode = FIELDIFNULL(InData["EventCode"]);

	std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
	std::string RequsetData = "?eventCode=" + EventCode;
	RequestAPI.append(RequsetData);
	auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
	if (Status != 200)return{};
	auto HttpReceiveInfo = Json::parse(Result);

	struct FEnteExecuteInfo
	{
	public:
		std::string EnteName = "";
		std::string PlanResp = "0";
		std::string RealResp = "0";
		std::string Percent = "0";
		std::string TargetId = "";

		static bool Sort(FEnteExecuteInfo A, FEnteExecuteInfo B)
		{
			return boost::lexical_cast<double>(A.RealResp) > boost::lexical_cast<double>(B.RealResp);
		}
	};
	std::vector<FEnteExecuteInfo> EnteExcuteInfoList;
	for (auto DataRow : HttpReceiveInfo["data"])
	{
		if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
		{
			FEnteExecuteInfo EnteExecuteInfo;
			EnteExecuteInfo.EnteName = FIELDIFNULL(DataRow["enteElecName"]);
			EnteExecuteInfo.TargetId = FIELDIFNULL(DataRow["targetId"]);

			EnteExecuteInfo.PlanResp = (FIELDIFNULL(DataRow["respFeedback"]));
			EnteExecuteInfo.RealResp = std::to_string(FIELDIFNULL_FLOAT(DataRow["realResp"]));
			if (EnteExecuteInfo.PlanResp != "" && EnteExecuteInfo.PlanResp != "0")
			{
				EnteExecuteInfo.Percent = SaveDecimalPlaces(std::to_string(lexical_cast<double>(EnteExecuteInfo.PlanResp) / lexical_cast<double>(EnteExecuteInfo.PlanResp) * 100));
			}
			EnteExcuteInfoList.push_back(EnteExecuteInfo);
		}
	}

	std::sort(EnteExcuteInfoList.begin(), EnteExcuteInfoList.end(), FEnteExecuteInfo::Sort);

	std::vector<Json> Tables;
	int Index = 1;
	for (FEnteExecuteInfo EnteExcuteInfoRow : EnteExcuteInfoList)
	{

		try
		{
			FMysqlResult RealCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoad WHERE TargetID='%1%' ORDER BY CreateDate;") % EnteExcuteInfoRow.TargetId));
			FMysqlResult BaseCurveResult = MaraidbConnector.Query(str(boost::format("SELECT CreateDate,DataValue FROM VppEventLoadBase WHERE TargetID='%1%' ORDER BY CreateDate;") % EnteExcuteInfoRow.TargetId));

			std::vector<Json> XLabelRangs;
			if (BaseCurveResult.ResultIsNotNull())
			{
				for (auto CurveRow : BaseCurveResult.DataTable)
				{
					Json XLabelRang;
					ptime Time = time_from_string(CurveRow[0]);
					std::string CurrentTime = to_simple_string(Time.time_of_day());
					CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
					XLabelRang["label"] = CurrentTime;
					XLabelRangs.push_back(XLabelRang);
				}
			}
			else
			{
				ptime XLabelTime = time_from_string("1970-01-01 00:00:00");
				for (int Index = 0; Index < 24; Index++)
				{
					std::string CurrentTime = to_simple_string(XLabelTime.time_of_day());
					CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
					Json XLabelRang;
					XLabelRang["label"] = CurrentTime;
					XLabelRangs.push_back(XLabelRang);
					XLabelTime = XLabelTime + hours(1);
				}
			}

			FMysqlResult Curve[2] = { RealCurveResult ,BaseCurveResult };
			std::string CategoryName[2] = { TCHAR_TO_UTF8("实际负荷"),TCHAR_TO_UTF8("基线") };
			std::string StartColor[2] = { "rgba(255,253,255,1)","rgba(0,253,173,1)" };
			std::string HighColor[2] = { "rgba(255,253,255,0.3)","rgba(97,173,255,0.3)" };
			std::string GradientColor[2] = { "rgba(0,253,173,0)","rgba(97,173,255,0)" };

			std::vector<Json> TableDatas;
			for (int CategoryIndex = 0; CategoryIndex < 2; CategoryIndex++)
			{
				std::vector<Json> CategoryDatas;
				for (auto CurveRow : Curve[CategoryIndex].DataTable)
				{
					ptime Time = time_from_string(CurveRow[0]);
					std::string CurrentTime = to_simple_string(Time.time_of_day());
					CurrentTime = CurrentTime.substr(0, CurrentTime.length() - 3);
					CategoryDatas.push_back({ {"xLabel",CurrentTime},{"yLabel",CurveRow[1]} });
				}

				Json TableData;
				TableData["categoryName"] = CategoryName[CategoryIndex];
				TableData["icon"] = "auto";
				TableData["lineStyle"] = "solid";
				TableData["start_color"] = StartColor[CategoryIndex];
				TableData["end_color"] = StartColor[CategoryIndex];
				TableData["high_color"] = HighColor[CategoryIndex];
				TableData["gradient_color"] = GradientColor[CategoryIndex];
				TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);
				TableDatas.push_back(TableData);
			}

			Json Table;
			Table["title"] = EnteExcuteInfoRow.EnteName;
			Table["unit"] = "kW";
			Table[TCHAR_TO_UTF8("计划量")] = EnteExcuteInfoRow.PlanResp;
			Table[TCHAR_TO_UTF8("完成量")] = EnteExcuteInfoRow.RealResp;
			Table[TCHAR_TO_UTF8("完成度")] = EnteExcuteInfoRow.Percent;
			Table[TCHAR_TO_UTF8("出力排行")] = std::to_string(Index) + "/" + std::to_string(EnteExcuteInfoList.size());
			Table["xLabelRang"] = XLabelRangs;
			Table["data"] = TableDatas;

			Tables.push_back(Table);
			Index++;
		}
		catch (const std::exception &)
		{

		}
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("企业出力曲线");
	ReturnData["table"] = Tables;

	return ReturnData;
}

Json OServerHelper::GetPlatformOverview(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult Area = MaraidbConnector.Query(str(boost::format("SELECT Area FROM BaseRegionInfo WHERE RegionID='310115';")));
	FMysqlResult EnteCount = MaraidbConnector.Query(str(boost::format("CALL GetRegionInfo_EnteNum('%1%')")%"310115"));
	FMysqlResult IndustryCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT IndustryID) FROM RegionIndustry WHERE FIND_IN_SET(RegionIndustry.RegionID,GetChildrenRegion('%1%'))")%"310115"));
	FMysqlResult Expert = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ExpertID) FROM ExpertInfo;")));

	FMysqlResult GoodsContractAmount = MaraidbConnector.Query(str(boost::format("SELECT SUM(ContractAmount) FROM GoodsContractInfo WHERE SignDate!='';")));
	FMysqlResult DemandContractAmount = MaraidbConnector.Query(str(boost::format("SELECT SUM(ContractAmount) FROM DemandContractInfo WHERE SignDate!='';")));

	std::string DealMoney = "--";

	if (GoodsContractAmount.ResultIsNotNull() && DemandContractAmount.ResultIsNotNull())
		DealMoney = ConvertToTenThousand(std::to_string(lexical_cast<double>(ConvertEmptyStringToZero(GoodsContractAmount.DataTable[0][0])) + lexical_cast<double>(ConvertEmptyStringToZero(DemandContractAmount.DataTable[0][0]))));

	Json Data;
	if (Area.ResultIsNotNull())
		Data["Area"] = Area.DataTable[0][0];
	if (Area.ResultIsNotNull())
		Data["EnteCount"] = EnteCount.DataTable[0][0];
	if (Area.ResultIsNotNull())
		Data["IndustryCount"] = IndustryCount.DataTable[0][0];
	if (Area.ResultIsNotNull())
		Data["ExpertCount"] = Expert.DataTable[0][0];

	Data["Deal"] = DealMoney;

	Json ReturnData;
	ReturnData["Data"] = Data;

	return ReturnData;
}

Json OServerHelper::DemandSideResponse(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult EventCountAndResp = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT EventCode),SUM(TotalComplete) FROM VppEvent;")));

	FMysqlResult EventList = MaraidbConnector.Query(str(boost::format("SELECT EventCode FROM VppEvent;")));

	int EnteCount = 0;

	for (auto EventRow : EventList.DataTable)
	{
		std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
		std::string RequsetData = "?eventCode=" + EventRow[0];
		RequestAPI.append(RequsetData);
		auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
		if (Status != 200)return{};
		auto HttpReceiveInfo = Json::parse(Result);

		for (auto DataRow : HttpReceiveInfo["data"])
		{
			if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
				EnteCount++;
		}
	}

	std::vector<Json> Datas;
	Datas.push_back({ {"name",TCHAR_TO_UTF8("累计削峰填谷事件")},{"value",EventCountAndResp.DataTable[0][0] + TCHAR_TO_UTF8("次")} });
	Datas.push_back({ {"name",TCHAR_TO_UTF8("累计参与企业")},{"value",std::to_string(EnteCount) + TCHAR_TO_UTF8("家")} });
	Datas.push_back({ {"name",TCHAR_TO_UTF8("累计响应量")},{"value",SaveDecimalPlaces(EventCountAndResp.DataTable[0][1]) + "kW"} });

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("需求侧响应");
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::PlatformServiceOverview(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	date Now = day_clock::local_day().end_of_month();
	std::vector<Json> XLabelConfig;

	month_iterator NowItreator(Now, -6);

	std::vector<Json> OrderCategoryDatas;
	std::vector<Json> EnteCategoryDatas;

	FMysqlResult TotleServiceCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT OrderCode),COUNT(DISTINCT EnteID) FROM ServerApply WHERE OrderStatus='已上传成果' ")));
	FMysqlResult TotleCustCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ApplyID),COUNT(DISTINCT ApplyEnteID) FROM CustApply WHERE ApplyStatus='编制完成' ")));
	FMysqlResult TotleAccCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ApplyID),COUNT(DISTINCT ApplyEnteID) FROM AccApply WHERE ApplyStatus='已开通'")));

	int TotleOrderCount = lexical_cast<int>(TotleServiceCount.DataTable[0][0]) + lexical_cast<int>(TotleCustCount.DataTable[0][0]) + lexical_cast<int>(TotleAccCount.DataTable[0][0]);
	int TotleEnteCount = lexical_cast<int>(TotleServiceCount.DataTable[0][1]) + lexical_cast<int>(TotleCustCount.DataTable[0][1]) + lexical_cast<int>(TotleAccCount.DataTable[0][1]);

	int Index = 0;
	while ((*NowItreator) > from_string("2020-06-01") && Index < 6)
	{
		std::string Date = to_iso_extended_string(*NowItreator);
		Date = Date.substr(0, Date.length() - 3);

		FMysqlResult ServiceCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT OrderCode),COUNT(DISTINCT EnteID) FROM ServerApply WHERE OrderStatus='已上传成果' AND PayDate<'%1%'") % Date));
		FMysqlResult CustCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ApplyID),COUNT(DISTINCT ApplyEnteID) FROM CustApply WHERE ApplyStatus='编制完成' AND PayTime<'%1%'") % Date));
		FMysqlResult AccCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(DISTINCT ApplyID),COUNT(DISTINCT ApplyEnteID) FROM AccApply WHERE ApplyStatus='已开通' AND PayTime<'%1%'") % Date));

		int OrderCount = lexical_cast<int>(ServiceCount.DataTable[0][0]) + lexical_cast<int>(CustCount.DataTable[0][0]) + lexical_cast<int>(AccCount.DataTable[0][0]);
		int EnteCount = lexical_cast<int>(ServiceCount.DataTable[0][1]) + lexical_cast<int>(CustCount.DataTable[0][1]) + lexical_cast<int>(AccCount.DataTable[0][1]);

		OrderCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",std::to_string(OrderCount)} });
		EnteCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",std::to_string(EnteCount)} });

		++NowItreator;
		Index++;
	}

	std::string CategoryNames[2] = { TCHAR_TO_UTF8("累计增值服务次数"),TCHAR_TO_UTF8("累计增值服务企业") };
	std::string StartColor[2] = { "rgba(1,254,234,0)","rgba(65,141,255,0)" };
	std::string EndColor[2] = { "rgba(1,254,234,1)","rgba(65,141,255,1)" };
	std::string Totals[2] = { std::to_string(TotleOrderCount),std::to_string(TotleEnteCount) };
	std::string Unit[2] = { TCHAR_TO_UTF8("次"),TCHAR_TO_UTF8("家") };
	std::vector<Json> CategoryDatas[2] = { OrderCategoryDatas ,EnteCategoryDatas };

	std::vector<Json> TableDatas;
	for (int i = 0; i < 2; i++)
	{
		Json TableData;
		TableData["categoryName"] = CategoryNames[i];
		TableData["start_color"] = StartColor[i];
		TableData["end_color"] = EndColor[i];
		TableData["high_color1"] = "#F2F2F2";
		TableData["high_color2"] = "#fce700";
		TableData["totals"] = Totals[i];
		TableData["unit"] = Unit[i];
		TableData["categoryData"] = CategoryDatas[i];

		TableDatas.push_back(TableData);
	}
	Json Table;
	Table["title"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(OrderCategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("服务金额年度对比");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::PlatformMarketOverview(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace boost::gregorian;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	date Now = day_clock::local_day().end_of_month();

	std::vector<Json> XLabelConfig;

	month_iterator NowItreator(Now, -6);

	std::vector<Json> OrderCategoryDatas;
	std::vector<Json> EnteCategoryDatas;

	FMysqlResult TotleDemandCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(ContractId),IFNULL(SUM(ContractAmount),0) FROM DemandContractInfo WHERE FlowStatus='已签订合同' ")));
	FMysqlResult TotleGoodsInfo = MaraidbConnector.Query(str(boost::format("SELECT COUNT(ContractId),IFNULL(SUM(ContractAmount),0) FROM GoodsContractInfo WHERE FlowStatus='已签订合同'")));


	int TotleDealCount = lexical_cast<int>(TotleDemandCount.DataTable[0][0]) + lexical_cast<int>(TotleGoodsInfo.DataTable[0][0]);
	double TotleDealMoney = lexical_cast<double>(TotleDemandCount.DataTable[0][1]) + lexical_cast<double>(TotleGoodsInfo.DataTable[0][1]);

	int Index = 0;
	while ((*NowItreator) > from_string("2020-06-01") && Index < 6)
	{
		std::string Date = to_iso_extended_string(*NowItreator);
		Date = Date.substr(0, Date.length() - 3);

		FMysqlResult DemandCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(ContractId),IFNULL(SUM(ContractAmount),0) FROM DemandContractInfo WHERE FlowStatus='已签订合同' AND SignDate <'%1%'") % Date));
		FMysqlResult GoodsCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(ContractId),IFNULL(SUM(ContractAmount),0) FROM GoodsContractInfo WHERE FlowStatus='已签订合同' AND SignDate <'%1%'") % Date));


		int DealCount = lexical_cast<int>(DemandCount.DataTable[0][0]) + lexical_cast<int>(GoodsCount.DataTable[0][0]);
		double DealMoney = lexical_cast<double>(DemandCount.DataTable[0][1]) + lexical_cast<double>(GoodsCount.DataTable[0][1]);

		OrderCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",std::to_string(DealCount)} });
		EnteCategoryDatas.push_back({ {"xLabel",Date},{"yLabel",ConvertToTenThousand(std::to_string(DealMoney))} });

		++NowItreator;
		Index++;
	}

	std::string CategoryNames[2] = { TCHAR_TO_UTF8("累计撮合交易"),TCHAR_TO_UTF8("累计交易金额") };
	std::string StartColor[2] = { "rgba(0,254,233,0)","rgba(65,141,255,0)" };
	std::string EndColor[2] = { "rgba(65,141,255,1)","rgba(65,141,255,1)" };
	std::string Totals[2] = { std::to_string(TotleDealCount),ConvertToTenThousand(std::to_string(TotleDealMoney)) };
	std::string Unit[2] = { TCHAR_TO_UTF8("笔"),TCHAR_TO_UTF8("万元") };
	std::vector<Json> CategoryDatas[2] = { OrderCategoryDatas ,EnteCategoryDatas };

	std::vector<Json> TableDatas;
	for (int i = 0; i < 2; i++)
	{
		Json TableData;
		TableData["categoryName"] = CategoryNames[i];
		TableData["start_color"] = StartColor[i];
		TableData["end_color"] = EndColor[i];
		TableData["high_color1"] = "#F2F2F2";
		TableData["high_color2"] = "#fce700";
		TableData["totals"] = Totals[i];
		TableData["unit"] = Unit[i];
		TableData["categoryData"] = CategoryDatas[i];

		TableDatas.push_back(TableData);
	}
	Json Table;
	Table["title"] = "";
	Table["xLabelRang"] = GetXLabelRangFromCategoryDatas(OrderCategoryDatas);
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("能源服务市场发展");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);


	FMysqlResult TotleEnteCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(EnteID) FROM EnteEvalvate; ")));
	FMysqlResult TotleGoodsCount = MaraidbConnector.Query(str(boost::format("SELECT COUNT(GoodsID) FROM GoodsInfo WHERE GoodsStatus!='审批中';")));

	std::vector<Json> Datas_1;
	Datas_1.push_back({ {"key",TCHAR_TO_UTF8("产业云联盟")},{"value",TotleEnteCount.DataTable[0][0] + TCHAR_TO_UTF8("家")} });
	Datas_1.push_back({ {"key",TCHAR_TO_UTF8("商品总量")},{"value",TotleGoodsCount.DataTable[0][0] + TCHAR_TO_UTF8("个")} });


	Json ReturnData;
	ReturnData["data"] = Datas;
	ReturnData["data1"] = Datas_1;

	return ReturnData;
}

Json OServerHelper::PeakCuttingAndValleyFillingEventTrend(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::string StartTime = FIELDIFNULL(InData["StartTime"]);
	std::string EndTime = FIELDIFNULL(InData["EndTime"]);

	FXLabelRangCofig Config = FXLabelRangCofig(1, true, true, false, '.', EXLabelStepDateIterator::Month);
	std::vector<Json> XLabelRangs = GetXLabelRang(StartTime, EndTime, Config);

	FMysqlResult EventTrend = MaraidbConnector.Query(str(boost::format("SELECT YEAR(StartDate),MONTH(StartDate),COUNT(EventCode) FROM VppEvent WHERE StartDate>'%1%' AND EndDate<'%2%';") % StartTime % EndTime));

	std::vector<Json> CategoryDatas;
	for (auto EventTrendRow : EventTrend.DataTable)
		CategoryDatas.push_back({ {"xLabel",EventTrendRow[0] + '.' + EventTrendRow[1]},{"yLabel",EventTrendRow[2]} });

	Json TableData;
	TableData["categoryName"] = TCHAR_TO_UTF8("事件数量");
	TableData["start_color"] = "rgb(96,199,255)";
	TableData["end_color"] = "rgb(96,199,255)";
	TableData["high_color"] = "rgba(96,199,255,0.4)";
	TableData["gradient_color"] = "rgba(96,199,255,0)";
	TableData["categoryData"] = FillCategoryData(XLabelRangs, CategoryDatas);

	std::vector<Json> TableDatas;
	TableDatas.push_back(TableData);

	Json Table;
	Table["title"] = "";
	Table["unit"] = TCHAR_TO_UTF8("次");
	Table["xLabelRang"] = XLabelRangs;
	Table["data"] = TableDatas;

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("近一年削峰填谷事件数量");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;
	return ReturnData;

}

Json OServerHelper::RespTopEnteList(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult EventList = MaraidbConnector.Query(str(boost::format("SELECT EventCode FROM VppEvent;")));

	struct FEnteRespInfo
	{
		std::string EnteName = "";
		float Resp = 0;
		static bool Sort(const FEnteRespInfo & A, const FEnteRespInfo & B)
		{
			return A.Resp > B.Resp;
		}
	};
	std::vector<FEnteRespInfo> EnteList;

	for (auto EventRow : EventList.DataTable)
	{
		std::string RequestAPI = HTTP_URL("/energyManage/getVppEventEnte");
		std::string RequsetData = "?eventCode=" + EventRow[0];
		RequestAPI.append(RequsetData);
		auto [ErrorCode, Status, Result] = cinatra::post(RequestAPI, "");
		if (Status != 200)return{};
		auto HttpReceiveInfo = Json::parse(Result);

		for (auto DataRow : HttpReceiveInfo["data"])
		{
			if ((FIELDIFNULL(DataRow["feedbackStatus"])) == TCHAR_TO_UTF8("同意邀约"))
			{
				std::string EnteName = FIELDIFNULL(DataRow["enteElecName"]);
				float Resp = FIELDIFNULL_FLOAT(DataRow["realResp"]);

				bool FindInList = false;
				for (FEnteRespInfo EnteListRow : EnteList)
				{
					if (EnteListRow.EnteName == EnteName)
					{
						EnteListRow.Resp += Resp;
						FindInList = true;
						break;
					}
				}

				if (!FindInList)
				{
					FEnteRespInfo NewEnte;
					NewEnte.EnteName = EnteName;
					NewEnte.Resp = Resp;
					EnteList.push_back(NewEnte);
				}


			}
		}

	}
	std::sort(EnteList.begin(), EnteList.end(), FEnteRespInfo::Sort);

	std::vector<Json> Datas;
	for (int i = 0; i < 10 && i < EnteList.size(); i++)
	{
		Datas.push_back({ {"name",EnteList[i].EnteName},{"value",SaveDecimalPlaces(std::to_string(EnteList[i].Resp))},{"unit","kW"} });
	}

	Json ReturnData;
	ReturnData["title"] = TCHAR_TO_UTF8("总响应量排行榜");
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::TempTest(Json InData)
{
	return { {"Temp",TCHAR_TO_UTF8("中文")} };
}

Json OServerHelper::HighLoadLine(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	FMysqlResult LineResult = MaraidbConnector.Query(str(boost::format("SELECT ElecLineName,VoltageLevel,StatElecName,LoadRate,YdayLoadRate,MonLoadRate FROM LineLoad WHERE LoadRate>50 AND LoadRate<150;")));


	std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("线路名称") ,TCHAR_TO_UTF8("电压等级"),TCHAR_TO_UTF8("所属电站"),TCHAR_TO_UTF8("线路负载率(%)"),TCHAR_TO_UTF8("昨日负载(%)") ,TCHAR_TO_UTF8("本月平均负载(%)") };
	std::vector<std::string > RowWidth = { "16.5","16.5","16.5","16.5","16.5","16.5" };

	Json Table = FillTableJson(RowHeader, RowWidth, LineResult.DataTable, 4);

	std::vector<Json> Tables;
	Tables.push_back(Table);

	Json Data;
	Data["title"] = TCHAR_TO_UTF8("重载线路");
	Data["table"] = Tables;

	std::vector<Json> Datas;
	Datas.push_back(Data);

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

Json OServerHelper::EB_High_LowLoadLine(Json InData)
{
	using namespace cinatra;
	using namespace boost;
	using namespace std;

	FMariadbConnectInfo MariadbConnectInfo(MARIADB_IP, MARIADB_USER, MARIADB_PASSWD, MARIADB_DATABASE, MARIADB_PORT);
	OMaraidbConnector MaraidbConnector(MariadbConnectInfo);
	MaraidbConnector.Connect();

	std::vector<Json> Datas;
	{
		FMysqlResult LineResult = MaraidbConnector.Query(str(boost::format("SELECT ElecLineName,VoltageLevel,StatElecName,LoadRate,MonLoadRate FROM LineLoad WHERE LoadRate>50 AND LoadRate<150;")));


		std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("线路名称") ,TCHAR_TO_UTF8("电压等级"),TCHAR_TO_UTF8("所属电站"),TCHAR_TO_UTF8("实时负载率") ,TCHAR_TO_UTF8("本月平均负载") };
		std::vector<std::string > RowWidth = { "20","20","20","20","20" };

		Json Table = FillTableJson(RowHeader, RowWidth, LineResult.DataTable, 4);

		std::vector<Json> Tables;
		Tables.push_back(Table);

		Json Data;
		Data["title"] = TCHAR_TO_UTF8("高负载线路");
		Data["table"] = Tables;
		Datas.push_back(Data);

	}
	{
		FMysqlResult LineResult = MaraidbConnector.Query(str(boost::format("SELECT ElecLineName,VoltageLevel,StatElecName,LoadRate,MonLoadRate FROM LineLoad WHERE LoadRate>0 AND LoadRate<30;")));


		std::vector<std::string > RowHeader = { TCHAR_TO_UTF8("线路名称") ,TCHAR_TO_UTF8("电压等级"),TCHAR_TO_UTF8("所属电站"),TCHAR_TO_UTF8("实时负载率") ,TCHAR_TO_UTF8("本月平均负载") };
		std::vector<std::string > RowWidth = { "20","20","20","20","20" };

		Json Table = FillTableJson(RowHeader, RowWidth, LineResult.DataTable, 4);

		std::vector<Json> Tables;
		Tables.push_back(Table);

		Json Data;
		Data["title"] = TCHAR_TO_UTF8("低负载线路");
		Data["table"] = Tables;
		Datas.push_back(Data);

	}

	Json ReturnData;
	ReturnData["data"] = Datas;

	return ReturnData;
}

std::vector<Json> OServerHelper::GetXLabelRang(std::string InStartTime, std::string InEndTime, FXLabelRangCofig XLabelRangCofig)
{
	using namespace boost::gregorian;

	switch (XLabelRangCofig.XLabelStepDateIterator)
	{
	case EXLabelStepDateIterator::Day:
		return GetXLabelRangInternal<day_iterator>(InStartTime, InEndTime, XLabelRangCofig);
	case EXLabelStepDateIterator::Month:
		return GetXLabelRangInternal<month_iterator>(InStartTime, InEndTime, XLabelRangCofig);
	case EXLabelStepDateIterator::Year:
		return GetXLabelRangInternal<year_iterator>(InStartTime, InEndTime, XLabelRangCofig);
	default:
		return GetXLabelRangInternal<day_iterator>(InStartTime, InEndTime, XLabelRangCofig);
	}
}

template<typename DateIterator>
std::vector<Json>
OServerHelper::GetXLabelRangInternal(std::string InStartTime, std::string InEndTime, FXLabelRangCofig XLabelRangCofig)
{
	using namespace boost::gregorian;
	std::vector<Json> XLabelRang;
	date StartTime = from_string(InStartTime);
	date EndTime = from_string(InEndTime);

	DateIterator EndTimeIterator(EndTime, XLabelRangCofig.StepLength);
	while (*EndTimeIterator >= StartTime)
		--EndTimeIterator;
	date AdjustStartTime = *(++EndTimeIterator);
	DateIterator StartTimeIterator(AdjustStartTime, XLabelRangCofig.StepLength);
	while (*StartTimeIterator <= EndTime)
	{
		std::string Date = "";
		if (XLabelRangCofig.bShowYear)
		{
			Date += std::to_string(StartTimeIterator->year());
			if (XLabelRangCofig.bShowMonth || XLabelRangCofig.bShowDay)
			{
				Date += XLabelRangCofig.IntervalChar;
			}
		}
		if (XLabelRangCofig.bShowMonth)
		{
			Date += std::to_string(StartTimeIterator->month());
			if (XLabelRangCofig.bShowDay)
			{
				Date += XLabelRangCofig.IntervalChar;
			}
		}
		if (XLabelRangCofig.bShowDay)
		{
			Date += std::to_string(StartTimeIterator->day());
		}
		Json Label;
		Label["label"] = Date;
		XLabelRang.push_back(Label);
		++StartTimeIterator;
	}
	return XLabelRang;
}

std::vector<Json> OServerHelper::GetXLabelRangFromCategoryDatas(std::vector<Json> InCategoryDatas)
{
	std::vector<Json> XLabelRangs;
	for (auto CategoryData : InCategoryDatas)
	{
		Json XLabelRang;
		XLabelRang["label"] = CategoryData["xLabel"];
		XLabelRangs.push_back(XLabelRang);
	}
	return XLabelRangs;
}

std::vector<Json> OServerHelper::FillCategoryData(std::vector<Json>XLabelRang, std::vector<Json> InCategoryDatas)
{
	std::vector<Json> OutDatas;
	for (auto XLabel : XLabelRang)
	{
		Json OutData;
		OutData["xLabel"] = XLabel["label"];
		OutData["yLabel"] = "0";
		for (auto CategoryData : InCategoryDatas)
		{
			if (CategoryData["xLabel"] == XLabel["label"])
			{
				OutData["yLabel"] = CategoryData["yLabel"];
				break;
			}
		}
		OutDatas.push_back(OutData);
	}
	return OutDatas;
}

std::vector<Json> OServerHelper::CombineCategoryData(std::vector<Json>XLabelRang, std::vector<Json>CategoryDatas_A, std::vector<Json> CategoryDatas_B, int DecimalPlacesNumber)
{
	std::vector<Json> OutDatas;
	for (auto XLabel : XLabelRang)
	{
		Json OutData;
		OutData["xLabel"] = XLabel["label"];
		OutData["yLabel"] = "0";
		for (auto CategoryData_A : CategoryDatas_A)
		{
			if (CategoryData_A["xLabel"] == XLabel["label"])
			{
				OutData["yLabel"] = CategoryData_A["yLabel"];
				break;
			}
		}
		for (auto CategoryData_B : CategoryDatas_B)
		{
			if (CategoryData_B["xLabel"] == XLabel["label"])
			{
				OutData["yLabel"] = SaveDecimalPlaces(std::to_string(boost::lexical_cast<double>(OutData["yLabel"].get<std::string>()) + boost::lexical_cast<double>(CategoryData_B["yLabel"].get<std::string>())), DecimalPlacesNumber);
				break;
			}
		}

		OutDatas.push_back(OutData);
	}
	return OutDatas;
}

std::vector<Json> OServerHelper::FiltrationSmallDataByRatio(std::vector<Json>CategoryDatas, float FiltrationRatio)
{
	double Sum = 0;
	for (auto CategoryData : CategoryDatas)
	{
		Sum += boost::lexical_cast<double>(CategoryData["yLabel"].get<std::string>());
	}
	double Other = 0;
	std::vector<Json> ReturnData;
	for (auto CategoryData : CategoryDatas)
	{
		double CurrentRatio = boost::lexical_cast<double>(CategoryData["yLabel"].get<std::string>()) / Sum;
		if (CurrentRatio > FiltrationRatio)
		{
			ReturnData.push_back(CategoryData);
		}
		else
		{
			Other += boost::lexical_cast<double>(CategoryData["yLabel"].get<std::string>());
		}
	}
	Json OtherCategoryData;
	OtherCategoryData["xLabel"] = TCHAR_TO_UTF8("其他");
	OtherCategoryData["yLabel"] = Other;

	ReturnData.push_back(OtherCategoryData);
	return ReturnData;
}

Json OServerHelper::FillTableJson(std::vector<std::string> InRowHeader, std::vector<std::string> RowWidth, std::vector<std::vector<std::string>> InTable, int MinRow /*= 4*/)
{
	std::vector<Json> RowHeaders;

	for (int i = 0; i < InRowHeader.size(); i++)
	{
		Json RowHeader;
		RowHeader["headerName"] = InRowHeader[i];
		RowHeader["width"] = RowWidth[i];
		RowHeaders.push_back(RowHeader);
	}

	std::vector<Json> TableDatas;

	for (auto TableRow : InTable)
	{
		std::vector<Json> TableData;
		for (auto TableRowValue : TableRow)
		{
			TableData.push_back({ {"value",TableRowValue} });
		}
		TableDatas.push_back(TableData);
	}
	while (TableDatas.size() < MinRow)
	{
		std::vector<Json> TableData;
		for (auto RowValue : RowHeaders)
		{
			std::string FillString = "";
			std::string Width = RowValue["width"];
			for (int i = 0; i < boost::lexical_cast<double>(Width); i++)
				FillString += "-";
			Json Value;
			Value["value"] = FillString;
			TableData.push_back(Value);
		}
		TableDatas.push_back(TableData);
	}

	Json Table;
	Table["rowHeader"] = RowHeaders;
	Table["data"] = TableDatas;

	return Table;
}
