#pragma once

#include <string>
#include <chrono>
#include <cassert>
#include <mpi.h>

#include <time.h>

#include "RBC.hpp"
//#include "common_mpi.hpp"

class DistributedTimer {
  public:
    DistributedTimer() = delete;
    DistributedTimer(const std::string& name)
        : time_(0), name_(name), started_(false) {}

    void start(MPI_Comm comm) {
        assert(!started_);
        started_ = true;
        MPI_Barrier(comm);
        start_ = MPI_Wtime();
    }

    void start(const RBC::Comm& comm) {
        assert(!started_);
        started_ = true;
        RBC::Barrier(comm);
        start_ = MPI_Wtime();
    }

    void stop() {
        time_ += MPI_Wtime() - start_;
        assert(started_);
        started_ = false;
    }

    void reset() {
        assert(!started_);
        time_ = 0;
    }

    double time() const { return time_; }

    std::string toString() const {
        return "\t" + name_ + "=" + std::to_string(time());
    }

    struct Max {
        Max(const std::string name, double max) : name_(name), max_(max) {}
        std::string toString() const {
            return "\t" + name_ + "=" + std::to_string(max_);
        }

        void printOnRoot(MPI_Comm& comm) {
            int myrank = -1;
            MPI_Comm_rank(comm, &myrank);
            if (myrank == 0) {
                std::cout << toString();
            }
        }
        void printOnRoot(RBC::Comm& comm) {
            int myrank = -1;
            RBC::Comm_rank(comm, &myrank);
            if (myrank == 0) {
                std::cout << toString();
            }
        }

      double time() const {
        return max_;
      }

      private:
        const std::string name_;
        const double max_;
    };

    DistributedTimer::Max max(MPI_Comm comm) {
        double dtime = time();
        double max;
        MPI_Allreduce(&dtime, &max, 1, MPI_DOUBLE, MPI_MAX, comm);
        reset();
        return Max(name_, max);
    }

    DistributedTimer::Max max(RBC::Comm comm) {
        double dtime = time();
        double max;
        RBC::Allreduce(&dtime, &max, 1, MPI_DOUBLE, MPI_MAX, comm);
        reset();
        return Max(name_, max);
    }

  private:
    double time_;
    std::string name_;
    bool started_;
    double start_;
};

class DistributedDummyTimer {
  public:
    DistributedDummyTimer() {}
    DistributedDummyTimer(std::string) {}

    void start(MPI_Comm&) const {}

    void start(RBC::Comm&) const {}

    void stop() const {}

    void reset() const {}

    size_t time() const { return 0; }

    std::string toString() const { return ""; }

    struct Max {
        std::string toString() { return ""; }
        void printOnRoot(MPI_Comm&) {}
        void printOnRoot(RBC::Comm&) {}
    };

    DistributedDummyTimer::Max max(MPI_Comm&) { return Max(); }
    DistributedDummyTimer::Max max(RBC::Comm&) { return Max(); }

  private:
};
