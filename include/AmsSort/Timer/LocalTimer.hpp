#pragma once

//#include "debug.h"
//#include "common.hpp"
//#include "common_mpi.hpp"
#include "RBC.hpp"
#include <assert.h>
#include <string>
#include <chrono>
#include <mpi.h>

class LocalTimer {
  public:
    LocalTimer() = delete;
    LocalTimer(const std::string& name)
        : time_(0), name_(name), started_(false) {}

    void start(MPI_Comm&) { start(); }
    void start(const RBC::Comm&) { start(); }
    void start() {
        assert(!started_);
        started_ = true;
        start_ = MPI_Wtime();
    }

    void stop() {
        time_ += MPI_Wtime() - start_;
	if (!started_) std::cout << "timer not started" << std::endl;
        assert(started_);
        started_ = false;
    }

    void reset() {
        assert(!started_);
        time_ = 0;
    }

    double time() const { return time_; }

    std::string toString() const {
        return "\t" + name_ + "=" + std::to_string(time_);
    }

    struct Max {
        Max(const std::string name, double max) : name_(name), max_(max) {}
        std::string toString() {
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

      private:
        const std::string name_;
        const double max_;
    };

    LocalTimer::Max max(MPI_Comm& comm) {
        double max;
        MPI_Allreduce(&time_, &max, 1, MPI_DOUBLE, MPI_MAX, comm);
        reset();
        return Max(name_, max);
    }
    LocalTimer::Max max(RBC::Comm& comm) {
        double max;
        RBC::Allreduce(&time_, &max, 1, MPI_DOUBLE, MPI_MAX, comm);
        reset();
        return Max(name_, max);
    }

  private:
    double time_;
    std::string name_;
    bool started_;
    double start_;
};

class LocalDummyTimer {
  public:
    LocalDummyTimer() {}
    LocalDummyTimer(std::string) {}

    void start(MPI_Comm) {}
    void start(RBC::Comm) {}

    void start() const {}

    void stop() const {}

    void reset() const {}

    size_t time() const { return 0; }

    std::string toString() const { return ""; }

    struct Max {
        std::string toString() { return ""; }
        void printOnRoot(MPI_Comm&) {}
        void printOnRoot(RBC::Comm&) {}
    };

    LocalDummyTimer::Max max(MPI_Comm&) { return Max(); }
    LocalDummyTimer::Max max(RBC::Comm&) { return Max(); }

  private:
};
