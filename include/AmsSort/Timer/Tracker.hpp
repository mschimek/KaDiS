#pragma once

#include "LocalTimer.hpp"
#include "DistributedTimer.hpp"
#include "LocalMeasure.hpp"

#include <string>

namespace Ams {

class DetailedTracker {
public:

  DetailedTracker()
    : various_t("various")
    , splitter_allgather_scan_t("splitter-allgather-scan")
    , overpartition_t("overpartition-solver")
    , exchange_t("exchange")
    , msg_assignment_t("msg-assignment")
    , sampling_t("sampling")
    , partition_t("partition")
    , split_comm_t("splitcomm")
    , local_sort_t("local-sort")
    , overpartition_repeats_c_("overpartition-repeats", MPI_UNSIGNED_LONG, 0)
    , receive_volume_c_("recvvolume", MPI_UNSIGNED_LONG, 0)
    , send_volume_c_("sendvolume", MPI_UNSIGNED_LONG, 0)
    , receive_messages_c_("recvmessages", MPI_UNSIGNED_LONG, 0)
    , send_messages_c_("sendmessages", MPI_UNSIGNED_LONG, 0)
    , splitters_c_("splitters", MPI_UNSIGNED_LONG, 0)
    , samples_c_("samples", MPI_UNSIGNED_LONG, 0)
  {}
    
  void max(MPI_Comm comm) {
    various_t.max(comm).printOnRoot(comm);
    splitter_allgather_scan_t.max(comm).printOnRoot(comm);
    overpartition_t.max(comm).printOnRoot(comm);
    exchange_t.max(comm).printOnRoot(comm);
    msg_assignment_t.max(comm).printOnRoot(comm);
    sampling_t.max(comm).printOnRoot(comm);
    partition_t.max(comm).printOnRoot(comm);
    split_comm_t.max(comm).printOnRoot(comm);
    local_sort_t.max(comm).printOnRoot(comm);
  }

  LocalTimer various_t;
  DistributedTimer splitter_allgather_scan_t;
  DistributedTimer overpartition_t;
  DistributedTimer exchange_t;
  DistributedTimer msg_assignment_t;
  DistributedTimer sampling_t;
  DistributedTimer partition_t;
  DistributedTimer split_comm_t;
  LocalTimer local_sort_t;
    
  LocalMeasure<size_t> overpartition_repeats_c_;
    
  /*
   * Received total number of elements per level.
   *
   */
  LocalMeasure<size_t> receive_volume_c_;

  /*
   * Send total number of elements per level.
   *
   */
  LocalMeasure<size_t> send_volume_c_;

  /*
   * Total number of received messages per level.
   *
   */
  LocalMeasure<size_t> receive_messages_c_;

  /*
   * Total number of send messages per level.
   *
   */
  LocalMeasure<size_t> send_messages_c_;

  /* 
   * Total number of splitters per level.
   *
   */
  LocalMeasure<size_t> splitters_c_;

  /* 
   * Total number of samples per level.
   *
   */
  LocalMeasure<size_t> samples_c_;
};

}
