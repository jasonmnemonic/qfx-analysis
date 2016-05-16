#include <iostream>

#include "Stochastic.hpp"
#include "Parser.hpp"
#include "Constants.hpp"

using namespace std;

Stochastic::Stochastic(Parser *parser, int buy_threshold, int sell_threshold,
	int fastk, int slowk, int slowd) : AbstractIndicator(parser) {

	// @TODO be more memory efficient with array sizing
	int max_candles = parser->get_max_candles();
	this->out_slowk = new double[max_candles];
	this->out_slowd = new double[max_candles];

	// @TODO allow for customisation of stochastic interpretation settings
	this->threshold_buy = buy_threshold; // typically 20
	this->threshold_sell = sell_threshold; // typically 80
	cout << "Buy threshold is <" << threshold_buy << endl;
	cout << "Sell threshold is >" << threshold_sell << endl;
	// @TODO allow for customisation of stochastic technical settings
	this->fastk_period = fastk;
	this->slowk_period = slowk;
	this->slowk_ma_type = TA_MAType_SMA;
	this->slowd_period = slowd;
	this->slowd_ma_type = TA_MAType_SMA;
}

Stochastic::~Stochastic() {
	delete out_slowk;
	delete out_slowd;
}

void Stochastic::process() {
	// we are up to bar 'index'
	const double *high_prices = parser->get_high_prices();
	const double *low_prices = parser->get_low_prices();
	const double *close_prices = parser->get_close_prices();
	int num_candles = parser->get_num_candles();
	// cout << "Called Stochastic::process()" << endl;

	// calculate stochastic for entire block
	TA_RetCode ret_code = TA_STOCH(0, num_candles - 1, high_prices, low_prices, close_prices,
		fastk_period, slowk_period, slowk_ma_type, slowd_period, slowd_ma_type, &out_begin,
		&out_num, out_slowk, out_slowd);
}

Signal Stochastic::get_signal(int index) {
	// account for the starting point of this indicator
	index -= out_begin;
	if (index < 0) {
		return NOT_APPLICABLE;
	}
	// otherwise we can proceed
	double d = *(out_slowd + index);
	// @TODO implement more complex stochastic analysis: use k, k/d crossovers, enter+exit overbought/oversold before deciding, etc.
	// @TODO do more research on why flipping the < and > appears to be so effective?!
	if (d <= threshold_buy) return BUY;
	if (d >= threshold_sell) return SELL;
	return NEUTRAL;
}

string Stochastic::get_desc() {
	return "STOCH(" + to_string(fastk_period) + "," + to_string(slowk_period) + "," + to_string(slowd_period) +
		"," + to_string(threshold_buy) + "," + to_string(threshold_sell) + ")";
}
