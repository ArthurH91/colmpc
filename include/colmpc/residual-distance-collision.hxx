///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2021-2022, LAAS-CNRS, University of Edinburgh, INRIA
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef COLMPC_RESIDUAL_HXX_
#define COLMPC_RESIDUAL_HXX_
#ifdef PINOCCHIO_WITH_HPP_FCL

#include "colmpc/residual-distance-collision.hpp"

namespace colmpc {
using namespace crocoddyl;

template <typename Scalar>
ResidualDistanceCollisionTpl<Scalar>::ResidualDistanceCollisionTpl(
    boost::shared_ptr<StateMultibody> state, const std::size_t nu,
    boost::shared_ptr<GeometryModel> geom_model,
    const pinocchio::PairIndex pair_id)
    : Base(state, 1, nu, true, false, false),
      pin_model_(*state->get_pinocchio()),
      geom_model_(geom_model),
      pair_id_(pair_id)
     {

  if (static_cast<pinocchio::FrameIndex>(geom_model_->collisionPairs.size()) <=
      pair_id) {
    throw_pretty(
        "Invalid argument: "
        << "the pair index is wrong (it does not exist in the geometry model)");
  }
}

template <typename Scalar>
ResidualDistanceCollisionTpl<Scalar>::~ResidualDistanceCollisionTpl() {}

template <typename Scalar>
void ResidualDistanceCollisionTpl<Scalar>::calc(
    const boost::shared_ptr<ResidualDataAbstract> &data,
    const Eigen::Ref<const VectorXs> &x, const Eigen::Ref<const VectorXs> &) {
  Data *d = static_cast<Data *>(data.get());

  // const Eigen::VectorBlock<const Eigen::Ref<const VectorXs>, Eigen::Dynamic> q =
  //     x.head(state_->get_nq());
  // computes the distance for the collision pair pair_id_

  const pinocchio::Model::JointIndex joint_id_1 = geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].first].parentJoint;
  const pinocchio::Model::JointIndex joint_id_2 = geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].second].parentJoint;

  if (joint_id_1 > 0) 
  {
  d->oMg_id_1 = d->pinocchio->oMi[joint_id_1] * geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].first].placement;
  }
  else 
  {
  d->oMg_id_1 =  geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].first].placement;
  }

  if (joint_id_2 > 0) 
  {
  d->oMg_id_2 = d->pinocchio->oMi[joint_id_2] * geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].second].placement;
  }
  else 
  {
  d->oMg_id_2 =  geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].second].placement;
  }


  // pinocchio::updateGeometryPlacements(pin_model_, *(d->pinocchio),
  //                                     *geom_model_.get(), d->geometry);

  // Check each corresponding element
  // for (int i = 0; i < d->oMg_id_1.rows(); ++i) {
  //     for (int j = 0; j < d->oMg_id_1.cols(); ++j) {
  //         if (d->oMg_id_1(i, j) != d->geometry.oMg[geom_model_->collisionPairs[pair_id_].first](i, j)) {
  //           throw_pretty(
  //               "Invalid matrix: "
  //               << "the value of the matrix d->oMg_id_1 is not the same as the one updated from geometryPlacements");
  //         }
  //       }
  // }
  
  // bool sameRotation1 = d->oMg_id_1.rotation().isApprox(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].first].rotation());
  // std::cout << "same rotation for oMg_id1? " << sameRotation1 << std::endl;

  // bool sameRotation2 = d->oMg_id_2.rotation().isApprox(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].second].rotation());
  // std::cout << "same rotation for oMg_id2? " << sameRotation2 << std::endl;

  // bool sameTranslation1 = d->oMg_id_1.translation().isApprox(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].first].translation());
  // std::cout << "same translation for oMg_id1? " << sameTranslation1 << std::endl;
  
  // bool sameTranslation2 = d->oMg_id_2.translation().isApprox(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].second].translation());
  // std::cout << "same translation for oMg_id2? " << sameTranslation2 << std::endl;

// const auto M1 = toFclTransform3f(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].first]);
  // d->r[0] = hpp::fcl::distance(
  //     geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].first].geometry.get(),
  //     toFclTransform3f(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].first]),
  //     geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].second].geometry.get(),
  //     toFclTransform3f(d->geometry.oMg[geom_model_->collisionPairs[pair_id_].second]), 
  //     d->req, 
  //     d->res
  // );
  d->r[0] = hpp::fcl::distance(
      geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].first].geometry.get(),
      toFclTransform3f(d->oMg_id_1),
      geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].second].geometry.get(),
      toFclTransform3f(d->oMg_id_2),
      d->req, 
      d->res
  );

}

template <typename Scalar>
void ResidualDistanceCollisionTpl<Scalar>::calcDiff(
    const boost::shared_ptr<ResidualDataAbstract> &data,
    const Eigen::Ref<const VectorXs> &x, const Eigen::Ref<const VectorXs> &) {
  Data *d = static_cast<Data *>(data.get());

  // const Eigen::VectorBlock<const Eigen::Ref<const VectorXs>, Eigen::Dynamic> q =
  //     x.head(state_->get_nq());

  const std::size_t nv = state_->get_nv();

  pinocchio::getFrameJacobian(
      pin_model_, *d->pinocchio,
      geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].first]
          .parentFrame,
      pinocchio::LOCAL_WORLD_ALIGNED, d->J1);

  pinocchio::getFrameJacobian(
      pin_model_, *d->pinocchio,
      geom_model_->geometryObjects[geom_model_->collisionPairs[pair_id_].second]
          .parentFrame,
      pinocchio::LOCAL_WORLD_ALIGNED, d->J2);


  // getting the nearest points belonging to the collision shapes
  // d->cp1 = d->res.nearest_points[0];
  const Vector3s & cp1 = d->res.nearest_points[0];
  // d->cp2 = d->res.nearest_points[1];
  const Vector3s & cp2 = d->res.nearest_points[1];

  // Transport the jacobian of frame 1 into the jacobian associated to cp1
  // Vector from frame 1 center to p1
  d->f1p1 = cp1 -
         d->pinocchio
             ->oMf[geom_model_
                       ->geometryObjects[geom_model_->collisionPairs[pair_id_]
                                             .first]
                       .parentFrame]
             .translation();
  d->f1Mp1.setIdentity(); // = pinocchio::SE3::Identity();
  d->f1Mp1.translation(d->f1p1);
  d->J1 = d->f1Mp1.toActionMatrixInverse() * d->J1;//todo change me for simpler
  // Transport the jacobian of frame 2 into the jacobian associated to cp2
  // Vector from frame 2 center to p2
  d->f2p2 = cp2 -
         d->pinocchio
             ->oMf[geom_model_
                       ->geometryObjects[geom_model_->collisionPairs[pair_id_]
                                             .second]
                       .parentFrame]
             .translation();
  d->f2Mp2.setIdentity();
  d->f2Mp2.translation(d->f2p2);
  d->J2 = d->f2Mp2.toActionMatrixInverse() * d->J2;

  // calculate the Jacobian
  // compute the residual derivatives
  //const auto diff = d->J1.template topRows<3>() - d->J2.template topRows<3>();
  data->Rx.leftCols(nv) = - d->res.normal.transpose() * ( d->J1.template topRows<3>() - d->J2.template topRows<3>());
  // std::cout << "cpp " <<data->Rx.leftCols(nv) << std::endl;
}
template <typename Scalar>
boost::shared_ptr<ResidualDataAbstractTpl<Scalar> >
ResidualDistanceCollisionTpl<Scalar>::createData(
    DataCollectorAbstract *const data) {
  return boost::allocate_shared<Data>(Eigen::aligned_allocator<Data>(), this,
                                      data);
}

template <typename Scalar>
const pinocchio::GeometryModel &
ResidualDistanceCollisionTpl<Scalar>::get_geometry() const {
  return *geom_model_.get();
}

}  // namespace colmpc

#endif  // PINOCCHIO_WITH_HPP_FCL
#endif  // PINOCCHIO_WITH_HPP_FCL
