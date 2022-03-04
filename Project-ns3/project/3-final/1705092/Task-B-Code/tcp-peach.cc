#include "tcp-peach.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpPeach");
NS_OBJECT_ENSURE_REGISTERED (TcpPeach);

TypeId
TcpPeach::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpPeach")
    .SetParent<TcpCongestionOps> ()
    .SetGroupName ("Internet")
    .AddConstructor<TcpPeach> ()
  ;
  return tid;
}

TcpPeach::TcpPeach (void) : TcpCongestionOps ()
{
  NS_LOG_FUNCTION (this);
}

TcpPeach::TcpPeach (const TcpPeach& sock)
  : TcpCongestionOps (sock)
{
  NS_LOG_FUNCTION (this);
}

TcpPeach::~TcpPeach (void)
{
}

uint32_t
TcpPeach::SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  if (segmentsAcked >= 1)
    {
      uint32_t sndCwnd = tcb->m_cWnd;
      tcb->m_cWnd = std::min ((sndCwnd + (segmentsAcked * tcb->m_segmentSize)), (uint32_t)tcb->m_ssThresh);
      NS_LOG_INFO ("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);
      return segmentsAcked - ((tcb->m_cWnd - sndCwnd) / tcb->m_segmentSize);
    }

  return 0;
}

// void
// TcpPeach::EnterRecovery(uint32_t currentDelivered)
// {
//   NS_LOG_FUNCTION (this);
//   NS_ASSERT (m_tcb->m_congState != TcpSocketState::CA_RECOVERY);

//   NS_LOG_DEBUG (TcpSocketState::TcpCongStateName[m_tcb->m_congState] <<
//                 " -> CA_RECOVERY");

//   if (!m_sackEnabled)
//     {
//       // One segment has left the network, PLUS the head is lost
//       m_txBuffer->AddRenoSack ();
//       m_txBuffer->MarkHeadAsLost ();
//     }
//   else
//     {
//       if (!m_txBuffer->IsLost (m_txBuffer->HeadSequence ()))
//         {
//           // We received 3 dupacks, but the head is not marked as lost
//           // (received less than 3 SACK block ahead).
//           // Manually set it as lost.
//           m_txBuffer->MarkHeadAsLost ();
//         }
//     }

//   // RFC 6675, point (4):
//   // (4) Invoke fast retransmit and enter loss recovery as follows:
//   // (4.1) RecoveryPoint = HighData
//   m_recover = m_tcb->m_highTxMark;
//   m_recoverActive = true;

//   m_congestionControl->CongestionStateSet (m_tcb, TcpSocketState::CA_RECOVERY);
//   m_tcb->m_congState = TcpSocketState::CA_RECOVERY;

//   // (4.2) ssthresh = cwnd = (FlightSize / 2)
//   // If SACK is not enabled, still consider the head as 'in flight' for
//   // compatibility with old ns-3 versions
//   uint32_t bytesInFlight = m_sackEnabled ? BytesInFlight () : BytesInFlight () + m_tcb->m_segmentSize;
//   m_tcb->m_ssThresh = m_congestionControl->GetSsThresh (m_tcb, bytesInFlight);

//   if (!m_congestionControl->HasCongControl ())
//     {
//       m_recoveryOps->EnterRecovery (m_tcb, m_dupAckCount, UnAckDataCount (), currentDelivered);
//       NS_LOG_INFO (m_dupAckCount << " dupack. Enter fast recovery mode." <<
//                   "Reset cwnd to " << m_tcb->m_cWnd << ", ssthresh to " <<
//                    m_tcb->m_ssThresh << " at fast recovery seqnum " << m_recover <<
//                    " calculated in flight: " << bytesInFlight);
//     }


//   // (4.3) Retransmit the first data segment presumed dropped
//   DoRetransmit ();
//   // (4.4) Run SetPipe ()
//   // (4.5) Proceed to step (C)
//   // these steps are done after the ProcessAck function (SendPendingData)
// }

void
TcpPeach::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  uint32_t w = tcb->m_cWnd / tcb->m_segmentSize;

  // Floor w to 1 if w == 0 
  if (w == 0)
    {
      w = 1;
    }

  NS_LOG_DEBUG ("w in segments " << w << " m_cWndCnt " << m_cWndCnt << " segments acked " << segmentsAcked);
  if (m_cWndCnt >= w)
    {
      m_cWndCnt = 0;
      tcb->m_cWnd += tcb->m_segmentSize;
      NS_LOG_DEBUG ("Adding 1 segment to m_cWnd");
    }

  m_cWndCnt += segmentsAcked;
  NS_LOG_DEBUG ("Adding 1 segment to m_cWndCnt");
  if (m_cWndCnt >= w)
    {
      uint32_t delta = m_cWndCnt / w;

      m_cWndCnt -= delta * w;
      tcb->m_cWnd += delta * tcb->m_segmentSize;
      NS_LOG_DEBUG ("Subtracting delta * w from m_cWndCnt " << delta * w);
    }
  NS_LOG_DEBUG ("At end of CongestionAvoidance(), m_cWnd: " << tcb->m_cWnd << " m_cWndCnt: " << m_cWndCnt);
}

void
TcpPeach::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  // Peach tcp_in_slow_start() condition
  if (tcb->m_cWnd < tcb->m_ssThresh)
    {
      NS_LOG_DEBUG ("In slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh);
      segmentsAcked = SlowStart (tcb, segmentsAcked);
    }
  else
    {
      NS_LOG_DEBUG ("In cong. avoidance, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh);
      CongestionAvoidance (tcb, segmentsAcked);
    }
}

std::string
TcpPeach::GetName () const
{
  return "TcpPeach";
}

uint32_t
TcpPeach::GetSsThresh (Ptr<const TcpSocketState> state,
                           uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << state << bytesInFlight);

  // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
  return std::max<uint32_t> (2 * state->m_segmentSize, state->m_cWnd / 2);
}

Ptr<TcpCongestionOps>
TcpPeach::Fork ()
{
  return CopyObject<TcpPeach> (this);
}

} // namespace ns3
