//  Portions Copyright 2010 Xuesong Zhou, Jason Lu

//   If you help write or modify the code, please also list your names here.
//   The reason of having Copyright info here is to ensure all the modified version, as a whole, under the GPL 
//   and further prevent a violation of the GPL.

// More about "How to use GNU licenses for your own software"
// http://www.gnu.org/licenses/gpl-howto.html

//    This file is part of DTALite.

//    DTALite is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    DTALite is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with DTALite.  If not, see <http://www.gnu.org/licenses/>.


// to do: network statistis have to be seprated 
				//// update network statistics


// prototype 1: heterogeneous traveler
// prototype 2: departure time and mode options
// prototype 3: extention

// assignment module
// obtain simulation results, fetch shortest paths, assign vehicles to the shortest path according to gap function or MSA

#include "stdafx.h"
#include "DTALite.h"
#include "GlobalData.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <algorithm>
#include <stdlib.h>  // Jason
#include <math.h>    // Jason


void g_OutputVOTStatistics()
{
/*
	int vot;
	for( vot = 0; vot<MAX_VOT_RANGE; vot++)
	{
	g_VOTStatVector[vot].TotalVehicleSize = 0;
	g_VOTStatVector[vot].TotalTravelTime = 0;
	g_VOTStatVector[vot].TotalDistance  = 0;

	}
	for (vector<DTAVehicle*>::iterator v = g_VehicleVector.begin(); v != g_VehicleVector.end();v++)
	{
	if((*v)->m_bComplete )  // vehicle completes the trips
	{
	vot = (*v)->m_VehData .m_VOT;
	g_VOTStatVector[vot].TotalVehicleSize +=1;
	g_VOTStatVector[vot].TotalTravelTime += (*v)->m_TripTime;
	g_VOTStatVector[vot].TotalDistance += (*v)->m_Distance;

	}
	}

	for(vot = 0; vot<MAX_VOT_RANGE; vot++)
	{
	if(g_VOTStatVector[vot].TotalVehicleSize > 0 )
	{
	g_AssignmentLogFile << "VOT= " << vot << ", # of vehicles = " << g_VOTStatVector[vot].TotalVehicleSize << ", Avg Travel Time = " << g_VOTStatVector[vot].TotalTravelTime/g_VOTStatVector[vot].TotalVehicleSize << ", Avg Distance = " << g_VOTStatVector[vot].TotalDistance /g_VOTStatVector[vot].TotalVehicleSize << endl;

	}
	}
	*/
}


/************************
//below is research code for multi-day traffic assignment, should not be included in the official release
		if(MAX_DAY_SIZE >=1)
		{
			g_MultiDayTrafficAssisnment();
		}

*************************/


	void g_MultiDayTrafficAssisnment()
{
/*
		int node_size  = g_NodeVector.size() +1 + g_ODZoneNumberSize;
		int link_size  = g_LinkVector.size() + g_NodeVector.size(); // maximal number of links including connectors assuming all the nodes are destinations

		g_LogFile << "Number of iterations = " << g_NumberOfIterations << endl;

		int iteration = 0;
		bool NotConverged = true;
		int TotalNumOfVehiclesGenerated = 0;



		for(unsigned li = 0; li< g_LinkVector.size(); li++)
		{
			g_LinkVector[li]->InitializeDayDependentCapacity();
		}

		for (vector<DTAVehicle*>::iterator vIte = g_VehicleVector.begin();vIte != g_VehicleVector.end();vIte++)
		{

			float RandomNumber= (*vIte)->GetRandomRatio();  // vehicle-dependent random number generator, very safe for multi-thread applications			
			if(RandomNumber < 0.05)  // percentage of PI users
				(*vIte)->m_bETTFlag = false;
			else
				(*vIte)->m_bETTFlag = true;
		}
		// 1st loop for each iteration
		for(iteration=0; NotConverged && iteration <= g_NumberOfIterations; iteration++)  // we exit from the loop under two conditions (1) converged, (2) reach maximum number of iterations
		{

			cout << "------- Iteration = "<<  iteration << "--------" << endl;

			// 2nd loop for each vehicle


			for(int day = 0; day < MAX_DAY_SIZE; day++)  // for VI users, we use iteration per day first then per vehicle, as vehicles share the same information per day
			{

		// output statistics
		if(bStartWithEmpty)
		{
			fprintf(st, "vehicle_id,from_zone_id,to_zone_id,departure_time,arrival_time,complete_flag,trip_time,demand_type,demand_type,vehicle_type,information_type,value_of_time,toll_cost_in_dollar,emissions,distance_in_mile,number_of_nodes,path_sequence\n");
		}

#pragma omp parallel for
				for (std::map<int, DTAZone>::iterator iterZone = g_ZoneMap.begin(); iterZone != g_ZoneMap.end(); iterZone++)
				{
					DTAZone *pZone = iterZone->second ;
					if(pZone.m_OriginVehicleSize>0)  // only this origin zone has vehicles, then we build the network
					{
						// create network for shortest path calculation at this processor
						DTANetworkForSP network_MP(node_size, link_size, g_PlanningHorizon,g_AdjLinkSize,g_DemandLoadingStartTimeInMin); //  network instance for single processor in multi-thread environment
						int	id = omp_get_thread_num( );  // starting from 0

						cout <<g_GetAppRunningTime()<<  "processor " << id << " is working on day " << day  << " PI assignment at zone  "<<  CurZoneID << endl;

						network_MP.BuildNetworkBasedOnZoneCentriod(CurZoneID);  // build network for this zone, because different zones have different connectors...

						for(unsigned li = 0; li< g_LinkVector.size(); li++)
						{
							float TravelTime;
							if(iteration == 0)
							{
								TravelTime = g_LinkVector[li]->m_FreeFlowTravelTime ;
							}
							else
							{
								TravelTime = g_LinkVector[li]->m_DayDependentTravelTime[day];
							}
							network_MP.m_LinkTDTimeAry[g_LinkVector[li]->m_LinkNo][0] = TravelTime;
//							network_MP.g_LinkTDCostAry[g_LinkVector[li]->m_LinkNo][0]= 0;
							// use travel time now, should use cost later
						}


						network_MP.TDLabelCorrecting_DoubleQueue(g_NodeVector.size(),0,1,DEFAULT_VOT,false,false);  // g_NodeVector.size() is the node ID corresponding to CurZoneNo

						for (int vi = 0; vi<g_TDOVehicleArray[g_ZoneMap[CurZoneID].m_ZoneSequentialNo][0].VehicleArray.size(); vi++)
						{

							int VehicleID = g_TDOVehicleArray[g_ZoneMap[CurZoneID].m_ZoneSequentialNo][0].VehicleArray[vi];
							DTAVehicle* pVeh  = g_VehicleMap[VehicleID];
							ASSERT(pVeh!=NULL);

							if(pVeh->m_bETTFlag == false)  // VI users
							{

								// create network for shortest path calculation at this processor

								bool bSwitchFlag = false;


								int PathNodeList[MAX_NODE_SIZE_IN_A_PATH]={-1};
								// get shortest path only when bSwitchFlag is true; no need to obtain shortest path for every vehicle
								int NodeSize = 0;
								int OriginCentriod = network_MP.m_PhysicalNodeSize;

								int DestinationCentriod = network_MP.m_PhysicalNodeSize+ pVeh->m_DestinationZoneID ;

								if( iteration >=1)
								{

									pVeh->m_DayDependentGap[day]= pVeh->m_DayDependentTripTime[day] - network_MP.LabelCostAry [DestinationCentriod];
								}else
								{
									pVeh->m_DayDependentGap[day] = 0;
								}

								float switching_rate = 1.0f/(iteration+1); // 0: MSA 

								if( iteration >=1)
								{
									switching_rate = (1.0f/(iteration+1)) * (pVeh->m_DayDependentGap[day] / pVeh->m_DayDependentTripTime[day]); // 3: Gap-based switching rule + MSA step size for UE
								}

								float RandomNumber= pVeh->GetRandomRatio();  // vehicle-dependent random number generator, very safe for multi-thread applications			

								if(RandomNumber < switching_rate || iteration==0)  			
								{
									bSwitchFlag = true;
								}				

								if(bSwitchFlag)  
								{
									int PredNode = network_MP.NodePredAry[DestinationCentriod];		
									while(PredNode != OriginCentriod && PredNode!=-1 && NodeSize< MAX_NODE_SIZE_IN_A_PATH) // scan backward in the predessor array of the shortest path calculation results
									{
										ASSERT(NodeSize< MAX_NODE_SIZE_IN_A_PATH-1);
										PathNodeList[NodeSize++] = PredNode;  // node index 0 is the physical node, we do not add OriginCentriod into PathNodeList, so NodeSize contains all physical nodes.
										PredNode = network_MP.NodePredAry[PredNode];
									}

									for(int i = 0; i< NodeSize-1; i++)
									{
										int LinkNo = network_MP.GetLinkNoByNodeIndex(PathNodeList[NodeSize-i-1], PathNodeList[NodeSize-i-2]);
										pVeh->m_DayDependentAryLink[day*MAX_PATH_LINK_SIZE+i]= LinkNo;
										pVeh->m_DayDependentNodeNumberSum[day] +=PathNodeList[i];

									}
									pVeh->m_DayDependentLinkSize[day] = NodeSize-1;
								}
							}
						}

					}
				}
			}

#pragma omp parallel for
			std::map<int, DTAZone>::iterator iterZone;
				for (iterZone = g_ZoneMap.begin(); iterZone != g_ZoneMap.end(); iterZone++)
				{
					DTAZone *pZone = iterZone->second ;
					if(pZone.m_OriginVehicleSize>0)  // only this origin zone has vehicles, then we build the network
				{
					// create network for shortest path calculation at this processor
					DTANetworkForSP network_MP(node_size, link_size, g_PlanningHorizon,g_AdjLinkSize,g_DemandLoadingStartTimeInMin); //  network instance for single processor in multi-thread environment
					int	id = omp_get_thread_num( );  // starting from 0

					cout <<g_GetAppRunningTime()<<  "processor " << id << " is working on ETT assignment at zone  "<<  CurZoneID << endl;

					network_MP.BuildNetworkBasedOnZoneCentriod(CurZoneID);  // build network for this zone, because different zones have different connectors...

					// 4th loop for each link cost
					for(unsigned li = 0; li< g_LinkVector.size(); li++)
					{
						float TravelTime;

						if(iteration == 0)
						{
							TravelTime = g_LinkVector[li]->m_FreeFlowTravelTime ;
						}
						else
						{
							TravelTime = g_LinkVector[li]->m_AverageTravelTime;
						}
						network_MP.m_LinkTDTimeAry[g_LinkVector[li]->m_LinkNo][0] = TravelTime;
//						network_MP.g_LinkTDCostAry[g_LinkVector[li]->m_LinkNo][0]=  TravelTime;
						// use travel time now, should use cost later
					}

					network_MP.TDLabelCorrecting_DoubleQueue(g_NodeVector.size(),0,1,DEFAULT_VOT,false,false);  // g_NodeVector.size() is the node ID corresponding to CurZoneNo


					for (int vi = 0; vi<g_TDOVehicleArray[g_ZoneMap[CurZoneID].m_ZoneSequentialNo][0].VehicleArray.size(); vi++)
					{

						int VehicleID = g_TDOVehicleArray[g_ZoneMap[CurZoneID].m_ZoneSequentialNo][0].VehicleArray[vi];
						DTAVehicle* pVeh  = g_VehicleMap[VehicleID];
						ASSERT(pVeh!=NULL);

						if(pVeh->m_bETTFlag == true)  // ETT users
						{

							int OriginCentriod = network_MP.m_PhysicalNodeSize;
							int DestinationCentriod = network_MP.m_PhysicalNodeSize+ pVeh->m_DestinationZoneID ;

							for(int day = 0; day < MAX_DAY_SIZE; day++)
							{
								if( iteration >=1)
								{
									pVeh->m_DayDependentGap[day]= pVeh->m_AvgDayTravelTime - network_MP.LabelCostAry [DestinationCentriod];

									if(pVeh->m_DayDependentGap[day] < -0.1)
									{
										g_AssignmentLogFile <<"iteration " << iteration << " gap < 0: " << pVeh->m_AgentID << " " << pVeh->m_AvgDayTravelTime << " " << network_MP.LabelCostAry [DestinationCentriod] <<endl;
									}
								}else
								{
									pVeh->m_DayDependentGap[day] = 0;
								}

							}
							bool bSwitchFlag = false;

							float switching_rate = 1.0f/(iteration+1); // 0: MSA 
							if( iteration >=1)
							{
								switching_rate = (1.0f/(iteration+1)) * (pVeh->m_AvgDayTravelTime - network_MP.LabelCostAry [DestinationCentriod]) / pVeh->m_AvgDayTravelTime; // 3: Gap-based switching rule + MSA step size for UE
							}
							float RandomNumber= pVeh->GetRandomRatio();  // vehicle-dependent random number generator, very safe for multi-thread applications			


							if(RandomNumber < switching_rate || iteration==0)  			
							{
								bSwitchFlag = true;
							}				

							// shortest path once, apply for all days
							int PathNodeList[MAX_NODE_SIZE_IN_A_PATH]={-1};
							// get shortest path only when bSwitchFlag is true; no need to obtain shortest path for every vehicle
							int NodeSize = 0;


							if(bSwitchFlag)  
							{
								int PredNode = network_MP.NodePredAry[DestinationCentriod];		
								while(PredNode != OriginCentriod && PredNode!=-1 && NodeSize< MAX_NODE_SIZE_IN_A_PATH) // scan backward in the predessor array of the shortest path calculation results
								{
									ASSERT(NodeSize< MAX_NODE_SIZE_IN_A_PATH-1);
									PathNodeList[NodeSize++] = PredNode;  // node index 0 is the physical node, we do not add OriginCentriod into PathNodeList, so NodeSize contains all physical nodes.
									PredNode = network_MP.NodePredAry[PredNode];
								}
								for(int i = 0; i< NodeSize-1; i++)
								{
									int LinkNo = network_MP.GetLinkNoByNodeIndex(PathNodeList[NodeSize-i-1], PathNodeList[NodeSize-i-2]);
									pVeh->m_DayDependentAryLink[0*MAX_PATH_LINK_SIZE+i]= LinkNo;
									pVeh->m_DayDependentNodeNumberSum[0] +=PathNodeList[i];
								}

								for(int day = 0; day < MAX_DAY_SIZE; day++)
								{
									pVeh->m_DayDependentLinkSize[day] = NodeSize-1;
								}
							}

						}
					}
				}
			}


			// below should be single thread

			cout << "---- Network Loading for Iteration " << iteration+1 <<"----" << endl;


			float DayByDayTravelTimeSumETT = 0;
			float DayByDayTravelTimeSumVI = 0;
			float PI_gap_sum = 0;
			float ETT_gap_sum = 0;
			float total_gap = 0;
			int VehicleCountVI = 0;
			float VehicleCountETT = 0;
			float TTSTDSumVI = 0;
			float TTSTDSumETT = 0;


			for(unsigned li = 0; li< g_LinkVector.size(); li++)
			{
				g_LinkVector[li]->m_AverageTravelTime = 0;
			}

			for (vector<DTAVehicle*>::iterator vIte = g_VehicleVector.begin();vIte != g_VehicleVector.end();vIte++)
			{
				(*vIte)->m_AvgDayTravelTime = 0;
			}

			for(int day = 0; day < MAX_DAY_SIZE; day++)
			{
				cout << "day:"<< day << "....."  << endl;

				for(unsigned li = 0; li< g_LinkVector.size(); li++)
				{

					g_LinkVector[li]->m_BPRLinkVolume = 0;
				}


				// switch data for the following line
				for(unsigned li = 0; li< g_LinkVector.size(); li++)
				{
					g_LinkVector[li]->m_BPRLaneCapacity = g_LinkVector[li]->m_DayDependentCapacity[day];
				}

				// fetch day-dependent vehicle path

				for (vector<DTAVehicle*>::iterator vIte = g_VehicleVector.begin();vIte != g_VehicleVector.end();vIte++)
				{
					for(int i = 0; i< (*vIte)->m_DayDependentLinkSize[day]; i++)
					{
						int LinkID;

						if((*vIte)->m_bETTFlag == true)
							LinkID= (*vIte)->m_DayDependentAryLink[0*MAX_PATH_LINK_SIZE+i];
						else
							LinkID= (*vIte)->m_DayDependentAryLink[day*MAX_PATH_LINK_SIZE+i];

						g_LinkVector[LinkID]->m_BPRLinkVolume++;
					}


				}


				// BPR based loading
				for(unsigned li = 0; li< g_LinkVector.size(); li++)
				{
					g_LinkVector[li]->m_BPRLinkTravelTime = g_LinkVector[li]->m_FreeFlowTravelTime*(1.0f+0.15f*(powf(g_LinkVector[li]->m_BPRLinkVolume/(g_LinkVector[li]->m_BPRLaneCapacity*g_LinkVector[li]->GetNumberOfLanes()),4.0f)));
					//				g_LogFile << "day:"<< day << ", BPR:"<< g_NodeVector[g_LinkVector[li]->m_FromNodeID] << " ->" << g_NodeVector[g_LinkVector[li]->m_ToNodeID]<<" Flow:" << g_LinkVector[li]->m_BPRLinkVolume << "travel time:" << g_LinkVector[li]->m_BPRLinkTravelTime  << endl;
					//				cout << "day:"<< day << ", BPR:"<< g_NodeVector[g_LinkVector[li]->m_FromNodeID] << " ->" << g_NodeVector[g_LinkVector[li]->m_ToNodeID]<<" Flow:" << g_LinkVector[li]->m_BPRLinkVolume << "travel time:" << g_LinkVector[li]->m_BPRLinkTravelTime  << endl;
					g_LinkVector[li]->m_DayDependentTravelTime[day] = g_LinkVector[li]->m_BPRLinkTravelTime;
					g_LinkVector[li]->m_AverageTravelTime+=  g_LinkVector[li]->m_BPRLinkTravelTime/MAX_DAY_SIZE;
				}

				VehicleCountETT = 0;
				float TravelTimeSumETT = 0;

				VehicleCountVI = 0;
				float TravelTimeSumVI = 0;


				for (vector<DTAVehicle*>::iterator vIte = g_VehicleVector.begin();vIte != g_VehicleVector.end();vIte++)
				{
					(*vIte)->m_DayDependentTripTime[day] = 0;

					if((*vIte)->m_bETTFlag == true)
					{
						for(int i = 0; i< (*vIte)->m_DayDependentLinkSize[0]; i++)
						{
							int LinkID = (*vIte)->m_DayDependentAryLink[0*MAX_PATH_LINK_SIZE+i];
							(*vIte)->m_DayDependentTripTime[day] += g_LinkVector[LinkID]->m_BPRLinkTravelTime;
						}

						(*vIte)->m_AvgDayTravelTime += ((*vIte)->m_DayDependentTripTime[day]/MAX_DAY_SIZE);

						TravelTimeSumETT+= (*vIte)->m_DayDependentTripTime[day];
						ETT_gap_sum += (*vIte)->m_DayDependentGap [day];
						total_gap +=(*vIte)->m_DayDependentGap [day];
						VehicleCountETT++;
					}else
					{
						for(int i = 0; i< (*vIte)->m_DayDependentLinkSize[day]; i++)
						{
							int LinkID = (*vIte)->m_DayDependentAryLink[day*MAX_PATH_LINK_SIZE+i];
							(*vIte)->m_DayDependentTripTime[day] += g_LinkVector[LinkID]->m_BPRLinkTravelTime;
						}

						(*vIte)->m_AvgDayTravelTime += ((*vIte)->m_DayDependentTripTime[day]/MAX_DAY_SIZE);


						TravelTimeSumVI+= (*vIte)->m_DayDependentTripTime[day];
						PI_gap_sum += (*vIte)->m_DayDependentGap [day];
						total_gap +=(*vIte)->m_DayDependentGap [day];
						VehicleCountVI++;
					}

				}



				float AverageTravelTimeETT = TravelTimeSumETT/max(VehicleCountETT,1);
				float AverageTravelTimeVI = TravelTimeSumVI/max(VehicleCountVI,1);


				DayByDayTravelTimeSumETT += AverageTravelTimeETT;
				DayByDayTravelTimeSumVI += AverageTravelTimeVI;


				//			g_AssignmentLogFile << "Iteration: " << iteration << ", Day: " << day <<  ", Ave ETT Travel Time: " << AverageTravelTimeETT << ", Ave VI Travel Time: " << AverageTravelTimeVI << ", # of VI: " << VehicleCountVI  <<   endl;
				cout << g_GetAppRunningTime() << "Iteration: " << iteration << ", Day: " << day <<  ", Ave ETT Travel Time: " << AverageTravelTimeETT << ", Ave VI Travel Time: " << AverageTravelTimeVI << ", # of VI: " << VehicleCountVI  <<  endl;
			}

			for (vector<DTAVehicle*>::iterator vIte = g_VehicleVector.begin();vIte != g_VehicleVector.end();vIte++)
			{
				float total_var = 0;
				for(int day = 0; day < MAX_DAY_SIZE; day++)
				{
					total_var += pow((*vIte)->m_DayDependentTripTime[day] - (*vIte)->m_AvgDayTravelTime, 2);
				}
				(*vIte)->m_DayTravelTimeSTD = sqrt(total_var/max(1,(MAX_DAY_SIZE-1)));

				if((*vIte)->m_bETTFlag == true)
				{
					TTSTDSumETT +=(*vIte)->m_DayTravelTimeSTD;
				}else
				{
					TTSTDSumVI +=(*vIte)->m_DayTravelTimeSTD;
				}
			}

			float AverageTTSTDETT = TTSTDSumETT/max(VehicleCountETT,1);
			float AverageTTSTDVI = TTSTDSumVI/max(VehicleCountVI,1);


			float Average_PI_Gap = PI_gap_sum/MAX_DAY_SIZE /max(VehicleCountVI,1);
			float Average_ETT_Gap = ETT_gap_sum/MAX_DAY_SIZE /max(VehicleCountETT,1);
			float Avg_gap = total_gap/MAX_DAY_SIZE/(VehicleCountETT+VehicleCountVI);
			g_AssignmentLogFile << "--D2D Summary: Iteration: " << iteration << ", Ave D2D ETT Travel Time: " << DayByDayTravelTimeSumETT/MAX_DAY_SIZE << ", Ave D2D VI Travel Time: " << DayByDayTravelTimeSumVI/MAX_DAY_SIZE << ", Ave ETT Gap: " << Average_ETT_Gap << ", Ave VI Gap: " << Average_PI_Gap << ", Ave Gap: " << Avg_gap << " TTSTDETT: "<< AverageTTSTDETT << ",TTSTDVI: "<< AverageTTSTDVI << endl;

			// we should output gap here
			//		g_AssignmentLogFile << g_GetAppRunningTime() << "Iteration: " << iteration << ", Ave Travel Time: " << SimuOutput.AvgTripTime << ", TTI: " << SimuOutput.AvgTTI  << ", Avg Distance: " << SimuOutput.AvgDistance  << ", Switch %:" << SimuOutput.SwitchPercentage <<", Num of Vehicles Completing Trips: ";

			//output vehicle trajectory data
			//	OutputLinkMOEData("LinkMOE.csv", iteration,bStartWithEmptyFile);
			//	OutputNetworkMOEData("NetworkMOE.csv", iteration,bStartWithEmptyFile);

		}

		//	OutputMultipleDaysVehicleTrajectoryData("MultiDayVehicle.csv");


	}
	void OutputMultipleDaysVehicleTrajectoryData(char fname[_MAX_PATH])
	{
		FILE* st = NULL;

		fopen_s(&st,fname,"w");

		if(st!=NULL)
		{
			std::map<int, DTAVehicle*>::iterator iterVM;
			int VehicleCount_withPhysicalPath = 0;

			fprintf(st, "vehicle_id,  originput_zone_id, destination_zone_id, departure_time, arrival_time, complete_flag, trip_time, demand_type, information_type, value_of_time, dollar_cost,emissions,distance_in_mile, number_of_nodes,  node id, node arrival time\n");


			for (vector<DTAVehicle*>::iterator vIte = g_VehicleVector.begin();vIte != g_VehicleVector.end();vIte++)
			{

				if((*vIte)->m_DayDependentLinkSize[0] >= 1)  // with physical path in the network
				{

					int UpstreamNodeID = 0;
					int DownstreamNodeID = 0;

					float TripTime = 0;

					if((*vIte)->m_bComplete)
						TripTime = (*vIte)->m_ArrivalTime-(*vIte)->m_DepartureTime;

					float m_gap = 0;
					fprintf(st,"%d,%d,%d,d%d,%4.2f,%4.2f,%d,%4.2f,%d,%d,%d,%4.1f,%4.2f,%4.2f,%d",
						(*vIte)->m_AgentID ,(*vIte)->m_bETTFlag, (*vIte)->m_OriginZoneID , (*vIte)->m_DestinationZoneID,
						(*vIte)->m_DepartureTime, (*vIte)->m_ArrivalTime , (*vIte)->m_bComplete, TripTime,
						(*vIte)->m_DemandType ,(*vIte)->m_InformationType, (*vIte)->m_VOT , (*vIte)->m_TollDollarCost, (*vIte)->CO2,(*vIte)->m_Distance, (*vIte)->m_NodeSize);

					fprintf(st, "\n AVG %5.3f, STD %5.3f,",(*vIte)->m_AvgDayTravelTime , (*vIte)->m_DayTravelTimeSTD) ;

					for(int day=0; day < MAX_DAY_SIZE; day++)
					{
						fprintf(st, "day %d,%5.2f,",day,(*vIte)->m_DayDependentTripTime[day]) ;

						for(int i = 0; i< (*vIte)->m_DayDependentLinkSize[day]; i++)
						{

							int LinkID = (*vIte)->m_DayDependentAryLink[day*MAX_PATH_LINK_SIZE+i];
							int NodeID = g_LinkVector[LinkID]->m_ToNodeID;
							int NodeName = g_NodeVector[NodeID].m_NodeNumber;

							fprintf(st, "%d",NodeName) ;
						}

						fprintf(st,"\n");

					}

				}
			}
			fclose(st);
		}
	*/
	}


/*
				if(g_TollingMethodFlag == 2)  // VMT toll
			{
				if(LinkID < g_LinkVector.size()) // physical link, which is always sort first.
				{
				DTALink* pLink= g_LinkVector[LinkID];
				 toll = pLink->m_Length * g_VMTTollingRate/max(1,VOT)*60;
				}
			}
*/




	void InnerLoopAssignment(int zone,int departure_time_begin, int departure_time_end, int inner_iteration) // this subroutine is called only when iteration > 0
{
		int PathNodeList[MAX_NODE_SIZE_IN_A_PATH]={-1};
		std::vector<DTAVehicle*>::iterator iterVehicle = g_VehicleVector.begin();
		int NodeSize;
		int AssignmentInterval = g_FindAssignmentIntervalIndexFromTime(departure_time_begin);  // starting assignment interval

		PathArrayForEachODT *PathArray;
		PathArray = new PathArrayForEachODT[g_ODZoneNumberSize + 1]; // remember to release memory

		ConstructPathArrayForEachODT(PathArray, zone, AssignmentInterval); 

		// loop through the TDOVehicleArray to assign or update vehicle paths...
		for (int vi = 0; vi<g_TDOVehicleArray[g_ZoneMap[zone].m_ZoneSequentialNo][AssignmentInterval].VehicleArray .size(); vi++)
		{
			int VehicleID = g_TDOVehicleArray[g_ZoneMap[zone].m_ZoneSequentialNo][AssignmentInterval].VehicleArray[vi];
			DTAVehicle* pVeh  = g_VehicleMap[VehicleID];
			ASSERT(pVeh!=NULL);

			bool bSwitchFlag = false;
			pVeh->m_bConsiderToSwitch = false;

			float m_gap;
			float ExperiencedTravelTime = pVeh->m_TripTime;	
			int VehicleDest = pVeh->m_DestinationZoneID;
			int NodeSum = pVeh->m_NodeNumberSum;

			int PathIndex = 0;		
			for(int p=1; p<=PathArray[VehicleDest].NumOfPaths; p++)
			{
				if(NodeSum == PathArray[VehicleDest].PathNodeSums[p])
				{
					PathIndex = p;
					break;
				}
			}

			float AvgPathTime = PathArray[VehicleDest].AvgPathTimes[PathIndex];		
			float MinTime = PathArray[VehicleDest].AvgPathTimes[PathArray[VehicleDest].BestPathIndex];		
			pVeh->SetMinCost(MinTime);

			if(g_VehicleExperiencedTimeGap == 1) 
				m_gap = ExperiencedTravelTime - MinTime;
			else
				m_gap = AvgPathTime - MinTime; 		

			if (m_gap < 0) m_gap = 0.0;
#pragma omp critical
			{
				g_CurrentGapValue += m_gap; // Jason : accumulate g_CurrentGapValue only when iteration >= 1
			}
			float switching_rate;
			// switching_rate = 1.0f/(iteration+1);   // default switching rate from MSA

			switch (g_UEAssignmentMethod)
			{
			case analysis_day_to_day_learning_threshold_route_choice: 
				
				switching_rate = 1.0f / (inner_iteration + 1);

				break;
			case analysis_gap_function_MSA_step_size: switching_rate = (1.0f/(inner_iteration+1)) * (m_gap / ExperiencedTravelTime); // 3: Gap-based switching rule + MSA step size for UE
				break;
			default: switching_rate = 1.0f/(inner_iteration+1); // default is MSA 
				break;
			}

			float RandomNumber= pVeh->GetRandomRatio();  // vehicle-dependent random number generator, very safe for multi-thread applications			

			if((pVeh->m_bComplete==false && pVeh->m_NodeSize >=2)) //for incomplete vehicles with feasible paths, need to switch at the next iteration
			{
				bSwitchFlag = true;
			}else
			{
				if(RandomNumber < switching_rate)  			
				{
					bSwitchFlag = true;
				}				
			} 

			if(bSwitchFlag)  
			{
				// accumulate number of vehicles switching paths
				g_CurrentNumOfVehiclesSwitched += 1; 

				pVeh->m_bConsiderToSwitch = true;

				// get shortest path only when bSwitchFlag is true; no need to obtain shortest path for every vehicle
				NodeSize = PathArray[VehicleDest].PathSize[PathArray[VehicleDest].BestPathIndex];			

				pVeh->m_NodeSize = NodeSize;

				if( pVeh->m_LinkAry !=NULL)
				{
					delete pVeh->m_LinkAry;
				}

				if(pVeh->m_NodeSize>=2)
				{
					pVeh->m_bConsiderToSwitch = true;

				if(NodeSize>=MAX_NODE_SIZE_IN_A_PATH)
				{
					cout << "PATH Size is out of bound: " << MAX_NODE_SIZE_IN_A_PATH << ": " << NodeSize;
						g_ProgramStop();
					}

					pVeh->m_LinkAry = new SVehicleLink[NodeSize];

					if(pVeh->m_LinkAry==NULL)
					{
						cout << "Insufficient memory for allocating vehicle arrays!";
						g_ProgramStop();
					}

					pVeh->m_NodeNumberSum = PathArray[VehicleDest].PathNodeSums[PathArray[VehicleDest].BestPathIndex];

					pVeh->m_Distance =0;

					for(int i = 0; i< NodeSize-1; i++)
					{
						pVeh->m_LinkAry[i].LinkNo = PathArray[VehicleDest].PathLinkSequences[PathArray[VehicleDest].BestPathIndex].LinkNoVector [i];					
						pVeh->m_Distance+= g_LinkVector[pVeh->m_LinkAry[i].LinkNo]->m_Length;
					}
					//cout << pVeh->m_AgentID <<  " Distance" << pVeh->m_Distance <<  endl;;
				}else
				{
					pVeh->m_bLoaded  = false;
					pVeh->m_bComplete = false;

					if(inner_iteration==0)
					{
						//					g_WarningFile  << "Warning: vehicle " <<  pVeh->m_AgentID << " from zone " << pVeh ->m_OriginZoneID << " to zone "  << pVeh ->m_DestinationZoneID << " does not have a physical path. Path Cost:" << TotalCost  << endl;
					}
				}
			} // if(bSwitchFlag)
		}

		delete PathArray;	
	}

	void g_DynamicTraffcAssignmentWithinInnerLoop(int iteration, bool NotConverged, int TotalNumOfVehiclesGenerated)
{
	if(iteration == 0) // Note: iteration 0 in outer loop only assigns initial paths to vehicles (i.e., without updating path assignment)
	{
		// do something?
	}else
	{
		// check if any vehicles swittching to new paths
		if((g_NewPathWithSwitchedVehicles == 0)||(iteration > g_NumberOfIterations)) //  g_NewPathWithSwitchedVehicles is determined in ZoneBasedPathAssignment
		{
			NotConverged = false; // converged for "outer" loop! 
		}else // run inner loop only when there are vehicles swittching to new paths found by TDSP
		{
			bool NotConvergedInner = true;

			// ----------* enter inner loop *----------
			for(int inner_iteration = 1; NotConvergedInner && inner_iteration <= g_NumberOfInnerIterations; inner_iteration++)
			{					
				// initialize for each iteration
				g_CurrentGapValue = 0.0;
				g_CurrentNumOfVehiclesSwitched = 0;
				g_CurrentNumOfVehiclesForUEGapCalculation = 0;

				for(int CurZoneID=1;  CurZoneID <= g_ODZoneNumberSize; CurZoneID++)
				{
				if(g_ZoneMap.find(CurZoneID) == g_ZoneMap.end())  // no such zone being defined
					continue;
					if(g_ZoneMap[CurZoneID].m_OriginVehicleSize>0)  // only this origin zone has vehicles, then we build the network
					{								
						// scan all possible departure times
						for (int departure_time_index = 0; departure_time_index < g_NumberOfSPCalculationPeriods; departure_time_index++)
						{

							int departure_time = g_AssignmentIntervalStartTimeInMin[departure_time_index];
							int  departure_end_time = g_AssignmentIntervalEndTimeInMin[departure_time_index];
							if (g_TDOVehicleArray[g_ZoneMap[CurZoneID].m_ZoneSequentialNo][departure_time_index].VehicleArray.size() > 0)
							{
								// update path assignments, g_CurrentGapValue, and g_CurrentNumOfVehiclesSwitched
								InnerLoopAssignment(CurZoneID, departure_time, departure_end_time, inner_iteration);
							}
						} // end - for each departure time interval
					}
				} // end - for each origin

				// evaluate new path assignments by simulation-based network loading
				cout << "---- Network Loading for Inner Loop Iteration " << inner_iteration <<"----" << endl;

				NetworkLoadingOutput SimuOutput;										
				SimuOutput = g_NetworkLoading(g_TrafficFlowModelFlag,0,iteration);

				TotalNumOfVehiclesGenerated = SimuOutput.NumberofVehiclesGenerated; // need this to compute avg gap

				g_AssignmentMOEVector[iteration] = SimuOutput;

				float PercentageComplete = 0;
				if(SimuOutput.NumberofVehiclesGenerated>0)
					PercentageComplete = SimuOutput.NumberofVehiclesCompleteTrips*100.0f/max(1,SimuOutput.NumberofVehiclesGenerated);

				g_LogFile << g_GetAppRunningTime() << "Inner Loop Iteration: " << inner_iteration << ", Average Trip Time: " << SimuOutput.AvgTripTime << ", TTI: " << SimuOutput.AvgTTI  << ", Average Distance: " << SimuOutput.AvgDistance << ", Switch %:" << SimuOutput.SwitchPercentage << ", Number of Vehicles Complete Their Trips: " <<  SimuOutput.NumberofVehiclesCompleteTrips<< ", " << PercentageComplete << "%"<<endl;
				cout << g_GetAppRunningTime() << "Inner Loop Iteration: " << inner_iteration <<", Average Trip Time: " << SimuOutput.AvgTripTime << ", Average Distance: " << SimuOutput.AvgDistance<< ", Switch %:" << SimuOutput.SwitchPercentage << ", Number of Vehicles Complete Their Trips: " <<  SimuOutput.NumberofVehiclesCompleteTrips << ", " << PercentageComplete << "%"<<endl;

				g_AssignmentLogFile << g_GetAppRunningTime() << "Inner Loop Iteration: " << inner_iteration << ", Ave Trip Time: " << SimuOutput.AvgTripTime << ", TTI: " << SimuOutput.AvgTTI  << ", Avg Distance: " << SimuOutput.AvgDistance  << ", Switch %:" << SimuOutput.SwitchPercentage <<", Num of Vehicles Completing Trips: " <<  SimuOutput.NumberofVehiclesCompleteTrips<< ", " << PercentageComplete << "%";			


				if(g_UEAssignmentMethod <= 1) // MSA and day-to-day learning
				{
					if(g_CurrentNumOfVehiclesSwitched < g_ConvergenceThreshold_in_Num_Switch)
						NotConvergedInner = false; // converged!
				}else // gap-based approaches
				{	
					//if(g_RelativeGap < g_ConvergencyRelativeGapThreshold_in_perc)
					//	NotConvergedInner = false; // converged! 
				}

				//g_PrevGapValue = g_CurrentGapValue; // update g_PrevGapValue

				float avg_gap = g_CurrentGapValue / max(1,TotalNumOfVehiclesGenerated);
				float avg_relative_gap = g_CurrentRelativeGapValue / max(1,TotalNumOfVehiclesGenerated) *100;
				 
				g_AssignmentLogFile << ", Num of Vehicles Switching Paths = " << g_CurrentNumOfVehiclesSwitched << ", Gap at prev iteration = " << g_CurrentGapValue << ", Avg Gap = " << avg_gap << ", Relative Gap at prev iteration = " << avg_relative_gap << "%" << endl;		

			} // end - enter inner loop
		} // end - check outer loop convergency				
	}
}


	void DTANetworkForSP::BuildHistoricalInfoNetwork(int CurZoneID, int CurrentTime, float Perception_error_ratio)  // build the network for shortest path calculation and fetch travel time and cost real-time data from simulator
{
	// example 
	// zones 1, 2, 3
	// CurZone ID = 2
	// add all physical links
	// add incoming links from its own (DES) nodes to CurZone ID = 2 
	// add outgoing links from  other zones 1 and 3 to each (DES) node
	// when finding the shortest path for a vehicle, start with origin zone, back trace to the CurZone as destination zone

	// build a network from the current zone centriod (1 centriod here) to all the other zones' centriods (all the zones)

	std::set<DTANode*>::iterator iterNode;
	std::set<DTALink*>::iterator iterLink;

	m_PhysicalNodeSize = g_NodeVector.size();

	int IntervalLinkID=0;
	int FromID, ToID;

	int i,t;

	for(i=0; i< m_PhysicalNodeSize + g_ODZoneNumberSize+1; i++)
	{
		m_OutboundSizeAry[i] = 0;
		m_InboundSizeAry[i] =0;
	}

	// add physical links

	for(unsigned li = 0; li< g_LinkVector.size(); li++)
	{
		FromID = g_LinkVector[li]->m_FromNodeID;
		ToID   = g_LinkVector[li]->m_ToNodeID;

		m_FromIDAry[g_LinkVector[li]->m_LinkNo] = FromID;
		m_ToIDAry[g_LinkVector[li]->m_LinkNo]   = ToID;

		//      TRACE("FromID %d -> ToID %d \n", FromID, ToID);
		m_OutboundNodeAry[FromID][m_OutboundSizeAry[FromID]] = ToID;
		m_OutboundLinkAry[FromID][m_OutboundSizeAry[FromID]] = g_LinkVector[li]->m_LinkNo ;
		m_OutboundSizeAry[FromID] +=1;

		m_InboundLinkAry[ToID][m_InboundSizeAry[ToID]] = g_LinkVector[li]->m_LinkNo ;
		m_InboundSizeAry[ToID] +=1;

		ASSERT(g_AdjLinkSize > m_OutboundSizeAry[FromID]);


			//TRACE("%d -> %d, time %d", g_NodeVector[g_LinkVector[li]->m_FromNodeID], g_NodeVector[g_LinkVector[li]->m_ToNodeID],CurrentTime);
			float AvgTripTime = g_LinkVector[li]->GetHistoricalTravelTime(CurrentTime);

			float Normal_random_value = g_RNNOF() * Perception_error_ratio*AvgTripTime;
			
			float travel_time  = AvgTripTime + Normal_random_value;
			if(travel_time < 0.1f)
				travel_time = 0.1f;
			//TRACE(" %6.3f zone %d \n",AvgTripTime, CurZoneID);

			m_LinkTDTimeAry[g_LinkVector[li]->m_LinkNo][0] = travel_time;
	}

	int LinkID = g_LinkVector.size();

		// add outgoing connector from the centriod corresponding to the current origin zone to physical nodes of the current zone
	for(i = 0; i< g_ZoneMap[CurZoneID].m_OriginActivityVector.size(); i++)
	{
		FromID = m_PhysicalNodeSize; // m_PhysicalNodeSize is the centriod number for CurZoneNo // root node
		ToID = g_ZoneMap[CurZoneID].m_OriginActivityVector [i];

		//         TRACE("destination node of current zone %d: %d\n",CurZoneID, g_NodeVector[ToID]);

		m_OutboundNodeAry[FromID][m_OutboundSizeAry[FromID]] = ToID;
		m_OutboundLinkAry[FromID][m_OutboundSizeAry[FromID]] = LinkID;
		m_OutboundSizeAry[FromID] +=1;

		m_InboundLinkAry[ToID][m_InboundSizeAry[ToID]] = LinkID ;
		m_InboundSizeAry[ToID] +=1;


		ASSERT(g_AdjLinkSize > m_OutboundSizeAry[FromID]);

		for(int t = 0; t <m_NumberOfSPCalculationIntervals; t++)
		{
			m_LinkTDTimeAry[LinkID][t] = 0;
		}

		LinkID++;

	}

	// add incoming connectors from the physicla nodes corresponding to a zone to the non-current zone.
		std::map<int, DTAZone>::iterator iterZone;
		for (iterZone = g_ZoneMap.begin(); iterZone != g_ZoneMap.end(); iterZone++)
		{
		DTAZone zone = iterZone->second ;
		if(iterZone->first !=CurZoneID)   // only this origin zone has vehicles, then we build the network
		{
			for(i = 0; i<  zone.m_OriginActivityVector.size(); i++)
			{
				FromID = zone.m_OriginActivityVector [i]; // m_PhysicalNodeSize is the centriod number for CurZoneNo
				ToID =   m_PhysicalNodeSize + iterZone->first; // m_PhysicalNodeSize is the centriod number for CurZoneNo, note that  .m_ZoneID start from 1

				m_OutboundNodeAry[FromID][m_OutboundSizeAry[FromID]] = ToID;
				m_OutboundLinkAry[FromID][m_OutboundSizeAry[FromID]] = LinkID;
				m_OutboundSizeAry[FromID] +=1;

					m_InboundLinkAry[ToID][m_InboundSizeAry[ToID]] = LinkID ;
					m_InboundSizeAry[ToID] +=1;


				ASSERT(g_AdjLinkSize >  m_OutboundSizeAry[FromID]);

				for( t= 0; t <m_NumberOfSPCalculationIntervals; t++)
				{
					m_LinkTDTimeAry[LinkID][t] = 0;
				}

				LinkID++;


			}
		}
	}

		m_NodeSize = m_PhysicalNodeSize + 1 + g_ODZoneIDSize;


}





// FREEVAL capacity allocation code

					//if(pLink -> m_bMergeFlag == 2 && g_UseFreevalRampMergeModelFlag)  // merge with onramp use freeval model
					//{
					//	// step a. check with flow on onramp
					//	bool OutFlowFlag = true;

					//	float MaxMergeCapacityForOnRamp = 
					//		g_LinkVector [ pLink->m_MergeOnrampLinkID ]->GetHourlyPerLaneCapacity(CurrentTime)*g_DTASimulationInterval/60.0f
					//		*g_LinkVector [ pLink->m_MergeOnrampLinkID ]->GetNumberOfLanes(DayNo,CurrentTime) * 0.5f; //60 --> cap per min --> unit # of vehicle per simulation interval
					//	// 0.5f -> half of the onramp capacity
					//	// use integer number of vehicles as unit of capacity

					//	int MaxMergeCapacity_int =  0;

					//	if(g_RandomizedCapacityMode)
					//	{
					//		MaxMergeCapacity_int = g_GetRandomInteger_From_FloatingPointValue_BasedOnLinkIDAndTimeStamp(MaxMergeCapacityForOnRamp,li); 
					//	}
					//	else
					//	{
					//		float PrevCumulativeMergeOutCapacityCount = pLink->m_CumulativeMergeOutCapacityCount;
					//		pLink->m_CumulativeMergeOutCapacityCount+= MaxMergeCapacityForOnRamp;
					//		MaxMergeCapacity_int = (int)pLink->m_CumulativeMergeOutCapacityCount - (int) PrevCumulativeMergeOutCapacityCount;
					//	}


					//	unsigned int DemandonOnRamp = g_LinkVector [ pLink->m_MergeOnrampLinkID ]->ExitQueue.size();
					//	int DownstreamLinkInCapacity = pLink->LinkInCapacity ;

					//	if(DemandonOnRamp > MaxMergeCapacity_int)  
					//		// ramp flow > max merge capacity on ramp  
					//		// over regions I and II in Dr. Rouphail's diagram
					//		// capacity on ramp = max merge capacity on ramp
					//		// capacity on main line = LinkInCapacity - capacity on ramp
					//		// if flow on main line > capacity on main line  // queue on main line
					//		// elsewise, no queue on mainline
					//	{  //region I
					//		g_LinkVector [ pLink->m_MergeOnrampLinkID ]->LinkOutCapacity = MaxMergeCapacity_int;
					//		g_LinkVector [pLink->m_MergeMainlineLinkID] ->LinkOutCapacity = DownstreamLinkInCapacity - MaxMergeCapacity_int;

					//	}else // ramp flow <= max merge capacity on ramp  // region III  
					//		// restrict the mainly capacity  // mainline capacity = LinkInCapacity - flow on ramp
					//	{
					//		g_LinkVector [ pLink->m_MergeOnrampLinkID ]->LinkOutCapacity = MaxMergeCapacity_int;
					//		g_LinkVector [pLink->m_MergeMainlineLinkID]->LinkOutCapacity = DownstreamLinkInCapacity - DemandonOnRamp;

					//	}
					//	if(pLink->m_FromNodeNumber == 58675 && pLink->m_ToNodeNumber == 57423 && CurrentTime>=1800)
					//	{
					//		TRACE("time %f, MaxMergeCapacityForOnRamp,DemandonOnRamp = %d, LinkOutCapacity = %, MaxMergeCapacity_int = %d\n", CurrentTime, MaxMergeCapacityForOnRamp, DemandonOnRamp, MaxMergeCapacity_int);
					//	}

					//	//		g_LogFile << "merge: mainline capacity"<< CurrentTime << " "  << g_LinkVector [pLink->m_MergeMainlineLinkID] ->LinkOutCapacity << endl;

					//}
