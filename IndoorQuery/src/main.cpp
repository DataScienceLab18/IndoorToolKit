#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <utility>
#include <map>
#include <limits>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/random/normal_distribution.hpp>

#include <boost/unordered_map.hpp>

#include "global.h"
#include "simulator.h"
#include "range_query.h"
#include "nearest_neighbor.h"

using std::cout;
using std::endl;

typedef boost::accumulators::accumulator_set<
  double, boost::accumulators::features<
            boost::accumulators::tag::mean,
  boost::accumulators::tag::variance> > accumulators;

static std::pair<int, int>
hit(const boost::unordered_map<int, double> &real,
    const boost::unordered_map<int, double> &fake,
    double thres = 0.0)
{
  int a = 0, b = 0;
  for (auto it = fake.cbegin(); it != fake.cend(); ++it) {
    if (it->second >= thres) {
      ++b;
      if (real.end() != real.find(it->first))
        ++a;
    }
  }
  return std::make_pair(a, b);
}

static double
recall(const boost::unordered_map<int, double> &real,
       const boost::unordered_map<int, double> &fake,
       double thres = 0.0)
{
  return 1.0 * hit(real, fake, thres).first / real.size();
}

static double
precision(const boost::unordered_map<int, double> &real,
          const boost::unordered_map<int, double> &fake,
          double thres = 0.0)
{
  if (real.size() == 0) return 0.0;
  std::pair<int, int> count = hit(real, fake, thres);
  return 0 == count.second ? 0.0 : 1.0 * count.first / count.second;
}

static std::pair<double, double>
score(const boost::unordered_map<int, double> &real,
      const boost::unordered_map<int, double> &fake,
      double threshold = 0.0)
{
  double res = 0.0;
  int cnt = 0;
  double e = std::numeric_limits<double>::epsilon();
  for (auto i = fake.begin(); i != fake.end(); ++i)
    if (i->second >= threshold &&
        real.find(i->first) != real.end()) {
      ++cnt;

      double p = real.at(i->first);
      double q = fake.at(i->first);

      if (p <= e)
        res += (1-p) * log((1-p)/(1-q));
      else if (1-p <= e || 1-q <= e)
        res += p * log(p/q);
      else res += p * log(p/q) + (1-p) * log((1-p)/(1-q));
    }
  return {res, cnt ? res / cnt : 0.0};
}

static double
score2(const boost::unordered_map<int, double> &m0,
       const boost::unordered_map<int, double> &m1)
{
  int tmp = 0;
  auto i0 = m0.begin();
  auto i1 = m1.begin();
  while (i0 != m0.end() && i1 != m1.end()) {
    if (i0->first < i1->first) ++i0;
    else if (i0->first > i1->first) ++i1;
    else ++tmp, ++i0, ++i1;
  }
  return m0.size() + m1.size() - tmp;
}

void
test_range_query()
{
  const double DURATION = 200;
  const int NUM_TEST = 100;

  using namespace simulation::param;
  simulation::Simulator sim(_num_object=200);

  sim.run(DURATION);

  simulation::RangeQuery rq(sim);

  boost::random::uniform_real_distribution<>
      unifd(DURATION / 2.0, DURATION * 3.0 / 4.0);

  const std::vector<double> Ratios = {.01, .02, .03, .04, .05,
                                      .06, .07, .08, .09};
  const std::vector<double> Threshold = {0, 1 // , .1, .15, .2, .25, .3,
                                         // .35, .4, .45
  };

  std::vector<std::vector<accumulators> > acc(
      Ratios.size(),
      std::vector<accumulators>(Threshold.size(), accumulators()));

  char msg[128];

  double e = std::numeric_limits<double>::epsilon();

  // boost::unordered_map<int, double> back;

  for (size_t i = 0; i < Ratios.size(); ++i) {
    for (int ts = 0; ts < NUM_TEST; ++ts) {

      while (true)
        if (rq.random_window(Ratios[i]))
          break;

      rq.prepare(unifd(gen));
      boost::unordered_map<int, double> real = rq.query();

      if (real.empty()) {
        --ts;
        continue;
      }
      boost::unordered_map<int, double> fake = rq.predict();

      std::pair<double, double> t = score(real, fake, Threshold[0]);
      if (t.first <= e) {
        --ts;
        continue;
      }

      acc[i][0](recall(real, fake));
      acc[i][1](t.second);
      // acc[i][1](recall(real, fake, Threshold[0]));
      // for (size_t j = 0; j < Threshold.size(); ++j)
      //   acc[i][j](score(real, fake, Threshold[j]));
      // back = real;
    }

    sprintf(msg, "%02lu / %02lu", i + 1, Ratios.size());
    cout << msg << endl;
  }

  std::ofstream fout("data.txt");
    for (size_t i = 0; i < Ratios.size(); ++i) {
      // for (size_t j = 0; j < Threshold.size(); ++j)
      fout << boost::accumulators::mean(acc[i][0]) << ' '
           << boost::accumulators::mean(acc[i][1]) << endl;;
      // fout << endl;
  }
  fout.close();
}

void
test_knn()
{
  const double DURATION = 200;
  const int NUM_TIMESTAMP = 100;
  const int N = 10;

  const std::vector<int> NumObject = {100, 200, 300, 400, 500,
                                      600, 700, 800, 900, 1000};

  std::vector<std::vector<accumulators> > acc(
      2, std::vector<accumulators>(NumObject.size(), accumulators()));

  for (size_t num = 0; num < NumObject.size(); ++num) {

    using namespace simulation::param;
    simulation::Simulator sim(_num_object=NumObject[num]);

    sim.run(DURATION);

    simulation::NearestNeighbor nn(sim);

    boost::random::uniform_real_distribution<>
        unifd(DURATION / 4.0, DURATION * 3.0 / 4.0);

    const double e = std::numeric_limits<double>::epsilon();

    for (int ts = 0; ts < NUM_TIMESTAMP; ++ts) {

      while (true) {

        nn.random_object();
        nn.prepare(unifd(gen));

        boost::unordered_map<int, double> real = nn.query(3);
        boost::unordered_map<int, double> fake = nn.predict(3);

        std::pair<double, double> t = score(real, fake);

        if (t.first <= e)
          continue;

        acc[0][num](recall(real, fake));
        acc[1][num](score(real, fake).second);

        break;
      }

      // cout << ts << '/' << NUM_TIMESTAMP << endl;
    }

    cout << num << '/' << NumObject.size() << endl;
  }

  std::ofstream fout("data.txt");
  for (size_t i = 0; i < NumObject.size(); ++i)
    fout << boost::accumulators::mean(acc[0][i]) << ' '
         << boost::accumulators::variance(acc[0][i]) << ' '
         << boost::accumulators::mean(acc[1][i]) << ' '
         << boost::accumulators::variance(acc[1][i]) << endl;
  fout.close();
}

void
test_threshold()
{
  const double DURATION = 200;
  const int NUM_TIMESTAMP = 100;

  using namespace simulation::param;
  simulation::Simulator sim(_num_object=200);

  sim.run(DURATION);

  simulation::NearestNeighbor nn(sim);

  accumulators acc0, acc1;
  boost::random::uniform_real_distribution<>
      unifd(DURATION / 4.0, DURATION * 3.0 / 4.0);

  for (int ts = 0; ts < NUM_TIMESTAMP; ++ts) {

    nn.random_object();

    nn.prepare(unifd(gen));

    boost::unordered_map<int, double> real = nn.query(3);
    boost::unordered_map<int, double> fake = nn.predict(3);

    for (auto i = fake.begin(); i != fake.end(); ++i) {
      if (real.find(i->first) != real.end())
        acc0(i->second);
      else
        acc1(i->second);
    }

    cout << ts << '/' << NUM_TIMESTAMP << endl;
  }

  cout << boost::accumulators::mean(acc0) << ' '
       << boost::accumulators::variance(acc0) << endl;

  cout << boost::accumulators::mean(acc1) << ' '
       << boost::accumulators::variance(acc1) << endl;
}

void
test_range_query_cont()
{
  const double DURATION = 400;
  const int MAX_TIMESTAMP = 30;
  const int NUM_TEST = 30;

  const std::vector<double> Ratios = {.01, .02, .03, .04, .05,
                                      .06, .07, .08, .09};

  using namespace simulation::param;
  simulation::Simulator sim(_num_object=200);

  sim.run(DURATION);

  simulation::RangeQuery rq(sim);

  std::vector<accumulators> acc(Ratios.size(), accumulators());

  double time = DURATION - MAX_TIMESTAMP;
  boost::random::uniform_real_distribution<>
      unifd(time / 2.0, time);

  boost::unordered_map<int, double> back;

  for (size_t i = 0; i < Ratios.size(); ++i) {
    for (int test = 0; test < NUM_TEST; ++test) {

      double start = unifd(gen);

      while (true) {
        if (rq.random_window(Ratios[i])) {
          rq.prepare(start);
          back = rq.query();

          if (!back.empty()) break;
        }
      }

      for (int ts = 1; ts < MAX_TIMESTAMP; ++ts) {

        rq.prepare(start + ts);
        boost::unordered_map<int, double> real = rq.query();

        if (real.empty()) continue;

        acc[i](score2(real, back));

        // boost::unordered_map<int, double> fake = rq.predict();

        // double tmp = recall(real, fake);
      }

      // if (tmp.size() > 0) {
      //   accumulators &acc = accs[tmp.size()];
      //   for (auto i = tmp.begin(); i != tmp.end(); ++i)
      //     acc(*i);
      // }

      cout << test << '/' << NUM_TEST << endl;
    }
  }

  std::ofstream fout("data.txt");
  for (auto i = acc.begin(); i != acc.end(); ++i)
    fout << boost::accumulators::mean(*i) << ' '
         << boost::accumulators::variance(*i) << endl;
  fout.close();
}

void
test_knn_cont()
{
  const double DURATION = 200;
  const int MAX_TIMESTAMP = 50;
  const int NUM_TEST = 100;

  using namespace simulation::param;
  simulation::Simulator sim(_num_object=200);

  sim.run(DURATION);

  simulation::NearestNeighbor nn(sim);

  accumulators acc[NUM_TEST];
  double time = DURATION - MAX_TIMESTAMP;
  boost::random::uniform_real_distribution<>
      unifd(time / 2.0, time);

  for (int test = 0; test < NUM_TEST; ++test) {

    double start = unifd(gen);
    nn.random_object();
    boost::unordered_map<int, double> back;

    for (int ts = 0; ts < MAX_TIMESTAMP; ++ts) {

      nn.prepare(start + ts);

      boost::unordered_map<int, double> real = nn.query(5);
      boost::unordered_map<int, double> fake = nn.predict(5);

      if (real != back) {
        acc[ts](recall(real, fake));
        back = real;
      }
    }

    cout << test << '/' << NUM_TEST << endl;
  }

  std::ofstream fout("data.txt");
  for (int i = 0; i < MAX_TIMESTAMP; ++i)
    fout << boost::accumulators::mean(acc[i]) << ' '
         << boost::accumulators::variance(acc[i]) << endl;
  fout.close();
}

int main()
{
  test_knn();
  return 0;
}
